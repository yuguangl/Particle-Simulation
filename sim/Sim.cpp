#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <windows.h>   // for MS Windows
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <algorithm>
#include <stdlib.h>
#include <thread>
#include <functional>
#include <future>
#include <omp.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "Variables.h"
#include "Particle.h"
#include "Shader.h"
#include "Boundry.h"


Particle particles[MAX_PARTICLES];


void GetParticleDensity( double xpos, double ypos) {
	//make this check the grid in the future
	float density;
	for (int i = 0; i < NUM_PARTICLES; i++) {
		if (GetDistance(particles[i].curr, vec2{ xpos, ypos }) <= RADIUS) {
			cout << i << " " << densities[i] << endl;
		}
	}
}

float GetParticleDistance(Particle p1) {
	vec2 diff = (p1.curr - p1.prev);
	return sqrt(diff.x * diff.x + diff.y * diff.y);
}

vec2 CalcDisplacement(Particle p1) {
	return p1.curr - p1.prev;
}


bool processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (GetAsyncKeyState(' ') && glfwGetTime() - prevTime > 0.2) {
		prevTime = glfwGetTime();
		if (!pause) {
			timeOffset = glfwGetTime();
		}
		else {
			glfwSetTime(timeOffset);
		}
		pause = !pause;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && glfwGetTime() - prevTime > 0.02) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		AddParticle(particles, xpos, ypos);
		//GetParticleDensity(particles, xpos, ypos);
		prevTime = glfwGetTime();
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && glfwGetTime() - prevTime > 0.1) {
		prevTime = glfwGetTime();
		if (!pause) {
			timeOffset = glfwGetTime();
		}
		else {
			glfwSetTime(timeOffset);
		}
		pause = !pause;
		return true;
	}
	
	return true;
}

void particleInvariantCheck() {
	int counter = 0;
	for (int i = 0; i < NUM_PARTICLES; i++) {
		if (particles[i].curr.x != particles[i].curr.x || particles[i].curr.x < 0 || particles[i].curr.x > WIDTH || particles[i].curr.y < 0 || particles[i].curr.y > HEIGHT) {
			counter++;
		}
	}
	if (counter > 0) {
		cout << "Particles Lost: " << counter << endl;
	}
}

void applyForces() {
	for (int i = 0; i < NUM_PARTICLES; i++) {
		particles[i].acc.y = G / (float)NUM_SUBSTEPS;
	}
}
static mutex m;
void CheckCollision(int j ,int i) {
	vec2 axis = { particles[i].curr.x - particles[j].curr.x, particles[i].curr.y - particles[j].curr.y };
	GLfloat dist = sqrt(axis.x * axis.x + axis.y * axis.y);
	vec2 norm;
	if (i != j) {
		if (dist < RADIUS * 2) {
			norm = { axis.x / dist, axis.y / dist };
			GLfloat delta = (RADIUS * 2) - dist;
			delta *= 0.75f;
			norm = { norm.x * delta * 0.5f, norm.y * delta * 0.5f };
			if (dist == 0) {
				int angle = rand() % 361;
				norm.x = cos(angle) * RADIUS;
				norm.y = sin(angle) * RADIUS;
				//lock_guard<mutex> lock(m);
				particles[i].curr += norm;
				particles[j].curr -= norm;

				//when they spawn in each other they have a lot of velocity
			}
			else {
				vec2 displacement = particles[i].curr - particles[i].prev;
				//lock_guard<mutex> lock(m);
				particles[i].curr += norm;
				particles[j].curr -= norm;
				particles[i].prev += displacement * 0.05f;
				particles[j].prev -= displacement * 0.05f;
			}
			

		}
	}
}


int getCellKey(int x, int y) {
	x = (x / CELL_SIZE);
	y = (y / CELL_SIZE);
	int hash = x * NUM_CELLS_X + y;//GetHashIndex(col, row) % (NUM_CELLS_X * NUM_CELLS_Y);//% NUM_PARTICLES;//maybe can recalculating is slow
	return hash;
	//make this take in a particle, use the particle index as the index to the hash and then just get the hash when getting cell particles
}

void AssignCell(const Particle& p, int i) {
	int x = p.curr.x / CELL_SIZE;
	int y = p.curr.y / CELL_SIZE;
	int hash = x * NUM_CELLS_X + y; //GetHashIndex(x, y) % (NUM_CELLS_X * NUM_CELLS_Y);// NUM_PARTICLES;
	hashes[i] = hash;
	Tuple t;
	t.hId = hash;
	t.pId = i;
	cellLookup[i] = t;
}

void PopulateGrid() {
	for (int i = 0; i < NUM_PARTICLES; i++) {
		AssignCell(particles[i], i);
	}
	//sorting slows down by a few frames, not sure if there are any better alternatives
	sort(cellLookup, cellLookup + NUM_PARTICLES);  
	fill_n(groupIndices, NUM_PARTICLES, -1);
	int prev = -1;
	for (int i = 0; i < NUM_PARTICLES; i++) {
		if (cellLookup[i].hId != prev) {
			prev = cellLookup[i].hId;
			groupIndices[prev] = i;
		}
	}
}

void GetCellParticles(int cellKey, int i) { //NEEDS TO BE RENAMED
	if (cellKey > (NUM_CELLS_X * NUM_CELLS_Y)-1 || cellKey < 0) {
		return;
	}
	int pos = groupIndices[cellKey];
	int prev = cellLookup[pos].hId;
	if (pos == -1) {
		return;
	}
	while (prev == cellLookup[pos].hId && pos < NUM_PARTICLES) {
		CheckCollision(cellLookup[pos].pId, i);
		pos++;
	}
	
}



void BruteForceCollisionCheck() {
	for (int i = 0; i < NUM_PARTICLES; i++) {
		for (int j = 0; j < NUM_PARTICLES; j++) {
			glm::vec2 axis = { particles[i].curr.x - particles[j].curr.x, particles[i].curr.y - particles[j].curr.y };
			GLfloat dist = sqrt(axis.x * axis.x + axis.y * axis.y);
			glm::vec2 norm;
			if (i !=j) {
				if (dist < RADIUS * 2) {
					norm = { axis.x / dist, axis.y / dist };
					GLfloat delta = (RADIUS * 2) - dist;
					delta *= 0.75f;
					norm = { norm.x * delta * 0.5f, norm.y * delta * 0.5f };
					if (dist == 0) {
						int angle = rand() % 361;
						norm.x = cos(angle) * RADIUS;
						norm.y = sin(angle) * RADIUS;
						particles[i].curr += norm;
						particles[j].curr -= norm;

						//when they spawn in each other they have a lot of velocity for some reason
					}
					else {
						glm::vec2 displacement = particles[i].curr - particles[i].prev;
						particles[i].curr += norm;
						particles[j].curr -= norm;
						particles[i].prev += displacement * 0.05f;
						particles[j].prev -= displacement * 0.05f;
					}

				}
			}
		}
	}
}

void AsyncFunction(int start, int end) {
	for (int i = start; i < end; i++) {
		int x = particles[i].curr.x;
		int y = particles[i].curr.y;
		int key = getCellKey(x, y);
		GetCellParticles(key, i);
		GetCellParticles(key - 1, i);
		GetCellParticles(key + 1, i);
		GetCellParticles(key - NUM_CELLS_X, i);
		GetCellParticles(key + NUM_CELLS_X, i);
		GetCellParticles(key + NUM_CELLS_X + 1, i);
		GetCellParticles(key + NUM_CELLS_X - 1, i);
		GetCellParticles(key - NUM_CELLS_X + 1, i);
		GetCellParticles(key - NUM_CELLS_X - 1, i);
	}
}
//vector<thread> threads;
//void createThread(int start, int end) {
//	threads.push_back(thread(AsyncFunction, start, end));
//	for (thread& t : threads) {
//		t.join();
//	}
//}


void ThreadedCollisions() {
	future<void> futures[NUM_THREADS];
	thread threads[NUM_THREADS];
	int start = 0;
	int incr = NUM_PARTICLES / NUM_THREADS;
	int end = incr;
	for (int i = 0; i < NUM_THREADS; i++) {
		//futures[i] = async(launch::async, createThread, start, end);
		futures[i] = (async(launch::async, AsyncFunction, start, end));
		//threads[i] = thread(AsyncFunction, start, end);
		start += incr;
		end += incr;
	}
	/*for(thread & t : threads) {
		t.join();
	}*/
}

void HandleCollisions() {
	for (int i = 0; i < NUM_PARTICLES; i++) {
		int x = particles[i].curr.x;
		int y = particles[i].curr.y;
		int key = getCellKey(x,y );
		GetCellParticles(key, i);
		GetCellParticles(key-1, i);
		GetCellParticles(key + 1, i);
		GetCellParticles(key - NUM_CELLS_X, i);
		GetCellParticles(key + NUM_CELLS_X, i);
		GetCellParticles(key + NUM_CELLS_X + 1, i);
		GetCellParticles(key + NUM_CELLS_X - 1, i);
		GetCellParticles(key - NUM_CELLS_X + 1, i);
		GetCellParticles(key - NUM_CELLS_X - 1, i);
	}
}


void updatePositions() {

	for (int i = 0; i < NUM_PARTICLES; i++) {
		vec2 displacement = particles[i].curr - particles[i].prev;
		particles[i].prev = particles[i].curr;
		particles[i].acc *= TIME_STEP * TIME_STEP;
		particles[i].curr += displacement + particles[i].acc;
		particles[i].acc = {};
	}
}

void checkBounds() {
	for (int i = 0; i < NUM_PARTICLES; i++) {
		vec2 displacement;
		if (particles[i].curr.y + RADIUS >= HEIGHT) {
			vec2 displacement = particles[i].curr - particles[i].prev;
			particles[i].curr.y = HEIGHT - RADIUS;

		}
		if (particles[i].curr.y - RADIUS <= 0) {
			vec2 displacement = particles[i].curr - particles[i].prev;
			particles[i].curr.y = RADIUS;
		}

		if (particles[i].curr.x + RADIUS >= WIDTH) {
			vec2 displacement = particles[i].curr - particles[i].prev;
			particles[i].curr.x = WIDTH - RADIUS;

		}
		if (particles[i].curr.x - RADIUS <= 0) {
			vec2 displacement = particles[i].curr - particles[i].prev;
			particles[i].curr.x = RADIUS;
		}
	}
}

void Update(GLFWwindow* window) {
	if (finalTime2 - initTime2 >= 1.0) {
		int fps = frames2;
		string f = "FPS: " + to_string(fps);
		const char* str_fps = (f).c_str();
		glfwSetWindowTitle(window, str_fps);

		particleInvariantCheck();
		frames2 = 0;
		initTime2 = glfwGetTime();
	}
	if (finalTime - initTime >= TIME_STEP) {
		for (int i = 0; i < NUM_SUBSTEPS; i++) {
			PopulateGrid();
			applyForces();
			HandleCollisions();
			//ThreadedCollisions();
			updatePositions();
			checkBounds();

		}
		initTime = glfwGetTime();
		frames2++;
	}
}


void BeginSim() {
	//create window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Starting Simulation...", NULL, NULL);

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, WIDTH, HEIGHT);

	//TODO: make a mesh instead the size of the particles doesnt change with the 
	// size of the window
	int n = (SEGMENTS * 3) + 3;
	

	Shader shader("VertexShader", "FragmentShader");
	Shader boundryShader("VertexShader", "FragmentShader");

	glfwSwapBuffers(window);

	GLuint VAO, VBO, EBO;
	//Generate Particles
	GLfloat* vertices = new GLfloat[n];
	GLuint* EBOIndices = new GLuint[n];
	GenerateParticle(vertices, EBOIndices, n);

	MakeParticleGrid(particles);
	MakeExtraParticles(particles);

	GLuint* buffers = CreateBuffers(vertices, EBOIndices, n);
	VAO = buffers[0], VBO = buffers[1], EBO = buffers[2];

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	int modelLoc = glGetUniformLocation(shader.shaderID, "model");
	int projLoc = glGetUniformLocation(shader.shaderID, "proj");
	int ColorLoc = glGetUniformLocation(shader.shaderID, "color");

	mat4 proj = glm::mat4(1.0f);
	proj = ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);

	initTime = glfwGetTime();
	initTime2 = glfwGetTime();

	for (int i = 0; i < NUM_PARTICLES; i++) {
		densities[i] = 0.0f;
		nearDensities[i] = 0.0f;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	shader.useShader();
	while (!glfwWindowShouldClose(window)) {
		bool nextFrame = processInput(window);
		if (!pause) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			for (int i = 0; i < NUM_PARTICLES; i++) {
				glm::mat4 model = glm::mat4(
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f, 
					particles[i].curr.x, particles[i].curr.y, 0.0f, 1.0f);
				float distance = GetParticleDistance(particles[i]);
				glUniform4f(ColorLoc, distance/5.0f, 0.0f, 5.0f / (distance), 1.0f);
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
				glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, 0); 
			}
			if (nextFrame) {
				finalTime = glfwGetTime();
				finalTime2 = glfwGetTime();
				Update(window);
			}
			glfwSwapBuffers(window);
		}
		glfwPollEvents();
	}

	//TODO: RUN VALGRIND
	//TODO: chemical reactions and stuff
	//TODO: https://www.benrogers.dev/
	//TODO: make 3D separate FILE
	///TODO: open second window and particles will flow into other window
	//TODO: particles react with window shake
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	shader.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return;
}

int main() {
	//Initialization
	srand(static_cast <unsigned> (time(0)));
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	prevTime = glfwGetTime();
	BeginSim();
	return 0;
}
