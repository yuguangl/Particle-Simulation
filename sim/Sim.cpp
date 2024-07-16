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

#include "Variables.h"
#include "Particle.h"
#include "Shader.h"
#include "Boundry.h"

vector<Particle> particles;


void GetParticleDensity( double xpos, double ypos) {
	//make this check the grid in the future
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
		for (int i = 0; i < NUM_PARTICLES; i++) {
			//cout << "circle[" << i << "]: (" << particles[i].curr.x << ", " << particles[i].curr.y << ")" << endl;
		}
		prevTime = glfwGetTime();
		if (!pause) {
			timeOffset = glfwGetTime();
		}
		else {
			glfwSetTime(timeOffset);
		}
		pause = !pause;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && glfwGetTime() - prevTime > 0.05) {
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
		//densities[i] = CalculateDensity(particles, i).x;
		//nearDensities[i] = CalculateDensity(particles, i).y;

		
		//TODO: HELPER FUNCTION THAT DETECTS WHERE SCREEN CLICK AND FIND THE PARTICLE AT THAT POINT
	}
	for (int i = 0; i < NUM_PARTICLES; i++) {
		//vec2 force = CalculateForce1(particles, i);
		
		/*if (densities[i] != 0) {
			particles[i].acc += ((force / densities[i]));
			if (fabs(particles[i].acc.y) > 500 || fabs(particles[i].acc.y) > 500) {
				cout << "force: " << force.x << " " << force.y << endl;
				cout << "density: " << densities[i] << endl;
			}
			
		}*/
		
		
	}
}

void CheckCollision(vector<int> & neighbors ,int i) {
	for (int j = 0; j < neighbors.size(); j++) {
		Particle p = particles[neighbors[j]];
		vec2 axis = { particles[i].curr.x - p.curr.x, particles[i].curr.y - p.curr.y };
		GLfloat dist = sqrt(axis.x * axis.x + axis.y * axis.y);
		vec2 norm;
		if (i != neighbors[j]) {
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
					p.curr -= norm;

					//when they spawn in each other they have a lot of velocity for some reason
				}
				else {
					vec2 displacement = particles[i].curr - particles[i].prev;
					particles[i].curr += norm;
					p.curr -= norm;
					//particles[i].prev += displacement * 0.05f;
					//p.prev -= displacement * 0.05f;
				}

			}
		}
	}
}


int getCellKey(int x, int y) {
	int col = (x / CELL_SIZE);
	int row = (int)(y / CELL_SIZE);
	int hash = GetHashIndex(col, row) % NUM_PARTICLES;
	return hash;
}

void AssignCell(const Particle p, int i) {
	int x = p.curr.x / CELL_SIZE;
	int y = p.curr.y / CELL_SIZE;
	int hash = GetHashIndex(x, y) % NUM_PARTICLES;
	Tuple t;
	t.hId = hash;
	t.pId = i;
	cellLookup[i] = t;
}

bool customSort(const Tuple& t1, const Tuple& t2) {
	return t1.hId < t2.hId;
}


void PopulateGrid() {
	
	for (int i = 0; i < NUM_PARTICLES; i++) {
		AssignCell(particles[i], i);
	}

	sort(cellLookup, cellLookup + NUM_PARTICLES, customSort);
	//FUCK WHAT IS GOING ON
	fill_n(groupIndices, NUM_PARTICLES, -1);
	int prev = -1;
	for (int i = 0; i < NUM_PARTICLES; i++) {
		if (cellLookup[i].hId != prev) {
			prev = cellLookup[i].hId;
			groupIndices[prev] = i;
		}
	}
}

vector<int> GetCellParticles(int x, int y) {
	int cellKey = getCellKey(x, y);
	int pos = groupIndices[cellKey];

	int prev = cellLookup[pos].hId;

	vector<int> cellNeighbors;
	if (pos == -1) {
		return cellNeighbors;
	}
	while (prev == cellLookup[pos].hId && pos < NUM_PARTICLES) {
		cellNeighbors.push_back(cellLookup[pos].pId);
		pos++;
	}
	return cellNeighbors;
}

void BruteForceCollisionCheck() {
	for (int i = 0; i < NUM_PARTICLES; i++) {
		for (int j = 0; j < NUM_PARTICLES; j++) {
			vec2 axis = { particles[i].curr.x - particles[j].curr.x, particles[i].curr.y - particles[j].curr.y };
			GLfloat dist = sqrt(axis.x * axis.x + axis.y * axis.y);
			vec2 norm;
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
						vec2 displacement = particles[i].curr - particles[i].prev;
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

void GetNeighbors(Particle p, int i) {

	int x = (int)p.curr.x;
	int y = (int)p.curr.y;
	vector<int> cellNeighbors;

	//cellNeighbors = GetCellParticles(x, y);
	//CheckCollision(cellNeighbors, i);
	/*cellNeighbors = GetCellParticles(x - CELL_SIZE, y);
	CheckCollision(cellNeighbors, i);
	cellNeighbors = GetCellParticles(x + CELL_SIZE, y);
	CheckCollision(cellNeighbors, i);
	cellNeighbors = GetCellParticles(x, y - CELL_SIZE);
	CheckCollision(cellNeighbors, i);
	cellNeighbors = GetCellParticles(x, y + CELL_SIZE);
	CheckCollision(cellNeighbors, i);
	cellNeighbors = GetCellParticles(x + CELL_SIZE, y + CELL_SIZE);
	CheckCollision(cellNeighbors, i);
	cellNeighbors = GetCellParticles(x + CELL_SIZE, y - CELL_SIZE);
	CheckCollision(cellNeighbors, i);
	cellNeighbors = GetCellParticles(x - CELL_SIZE, y - CELL_SIZE);
	CheckCollision(cellNeighbors, i);
	cellNeighbors = GetCellParticles(x - CELL_SIZE, y + CELL_SIZE);
	CheckCollision(cellNeighbors, i);*/

}


void HandleCollisions() {
	for (int i = 0; i < NUM_PARTICLES; i++) {
		GetNeighbors(particles[i], i);
	}
}

void updatePositions() {

	for (int i = 0; i < NUM_PARTICLES; i++) {
		vec2 displacement = particles[i].curr - particles[i].prev;
		particles[i].prev = particles[i].curr;
		particles[i].acc *= TIME_STEP * TIME_STEP;
		//cout << particles[i].acc.x << endl;
		particles[i].curr += displacement + particles[i].acc;
		particles[i].acc = {0,0};
	}
}

void checkBounds() {
	for (int i = 0; i < NUM_PARTICLES; i++) {
		//IF THE WALLS ARE BEING WEIRD GET RID OF .PREV MODIFICATIONS
		vec2 displacement;
		if (particles[i].curr.y + RADIUS >= HEIGHT) {
			vec2 displacement = particles[i].curr - particles[i].prev;
			particles[i].curr.y = HEIGHT - RADIUS;
			//particles[i].prev.y = particles[i].curr.y + displacement.y * 0.9f;

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
	//check fps
	if (finalTime2 - initTime2 >= 1.0) {
		int fps = frames2;
		string f = "FPS: " + to_string(fps);
		const char* str_fps = (f).c_str();
		// I dont think this is correct
		glfwSetWindowTitle(window, str_fps);

		//check particles lost
		particleInvariantCheck();
		frames2 = 0;
		initTime2 = glfwGetTime();
	}
	if (finalTime - initTime >= TIME_STEP) {
		for (int i = 0; i < NUM_SUBSTEPS; i++) {
			//PopulateGrid();
			applyForces();
			BruteForceCollisionCheck();
			//HandleCollisions();
			checkBounds();
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

	//generate particles
	//vector<Particle> particles;
	int n = (SEGMENTS * 3) + 3;
	

	Shader shader("VertexShader", "FragmentShader");
	Shader boundryShader("VertexShader", "FragmentShader");

	glfwSwapBuffers(window);

	GLuint* VAO = new GLuint[MAX_PARTICLES];
	GLuint* VBO = new GLuint[MAX_PARTICLES];
	GLuint* EBO = new GLuint[MAX_PARTICLES];

	MakeParticleGrid(particles);
	MakeExtraParticles(particles);

	for (int i = 0; i < MAX_PARTICLES; i++) {
		GLuint* buffers = CreateBuffers(particles[i].vertices, particles[i].EBOIndices, n);
		VAO[i] = buffers[0], VBO[i] = buffers[1], EBO[i] = buffers[2];
	}
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	int modelLoc = glGetUniformLocation(shader.shaderID, "model");
	int projLoc = glGetUniformLocation(shader.shaderID, "proj");
	int ColorLoc = glGetUniformLocation(shader.shaderID, "color");

	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);

	initTime = glfwGetTime();
	initTime2 = glfwGetTime();

	for (int i = 0; i < NUM_PARTICLES; i++) {
		densities.push_back(0.0f);
		nearDensities.push_back(0.0f);
	}
	PopulateGrid();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	while (!glfwWindowShouldClose(window)) {
		bool nextFrame = processInput(window);
		shader.useShader();
		if (!pause) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			for (int i = 0; i < NUM_PARTICLES; i++) {
				glBindVertexArray(VAO[i]);
				glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, vec3(particles[i].curr.x, particles[i].curr.y, 0.0f));
				float distance = GetParticleDistance(particles[i]);
				glUniform4f(ColorLoc, distance/10.0f, 0.0f, 10.0f / (distance), 1.0f);
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
				
				glBufferData(GL_ARRAY_BUFFER, n * sizeof(GLfloat), particles[i].vertices, GL_STATIC_DRAW);

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
	//TOD): MAKE PARTICLES PUBLIC
	for (int i = 0; i < NUM_PARTICLES; i++) {
		glDeleteVertexArrays(1, &VAO[i]);
		glDeleteBuffers(1, &VBO[i]);
		glDeleteBuffers(1, &EBO[i]);
	}
	shader.Delete();
	glfwDestroyWindow(window);
	for (int i = 0; i < NUM_PARTICLES; i++) {
		glDeleteBuffers(1, particles[i].EBOIndices);
		delete[] particles[i].vertices;
	}
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
