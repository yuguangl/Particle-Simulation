#pragma once
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
#include <unordered_map>
#include <thread>
//#include <pthread.h>

#include "Variables.h"
#include "Particle.h"
#include "Shader.h"
#include "Boundry.h"



bool processInput(GLFWwindow* window, const vector<Particle>& particles) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (GetAsyncKeyState(' ') && glfwGetTime() - prevTime > 0.2) {
		for (int i = 0; i < NUM_CIRCLES; i++) {
			cout << "circle[" << i << "]: (" << particles[i].curr.x << ", " << particles[i].curr.y << ")" << endl;
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
	else {
		return true;
	}
}

void particleInvariantCheck(const vector<Particle> &particles){
	int counter = 0;
	for (int i = 0; i < NUM_CIRCLES; i++) {
		if (particles[i].curr.x != particles[i].curr.x ||particles[i].curr.x < 0 || particles[i].curr.x > WIDTH || particles[i].curr.y < 0 || particles[i].curr.y > HEIGHT) {
			counter++;
		}
	}
	if (counter > 0) {
		cout << "Particles Lost: " << counter << endl;
	}
}

void applyG(vector<Particle> &particles) {
	for (int i = 0; i < NUM_CIRCLES; i++) {
		particles[i].acc.y = -G* SUBSTEP;
	}
}

glm::vec2 getCellID(const Particle p) {
	int x = (p.curr.x / 100);
	int y = (p.curr.y / 100);
	//glm::vec2 cellID = {
	return glm::vec2(x, y);
}

void divideSpace(vector<Particle>& particles) {
	spaceMap.clear();
	vector<int> v;
	for (int i = 0; i < NUM_CELL_X; i++) {
		for (int j = 0; j < NUM_CELL_Y; j++) {
			spaceMap.insert(make_pair(glm::vec2(i, j), v));
		}
	}
	for (int i = 0; i < particles.size(); i++) {
		glm::vec2 cellID = getCellID(particles[i]);
		spaceMap[cellID].push_back(i);
	}
}

vector<int> GetSameCellParticles(glm::vec2 cell) {
	return spaceMap[cell];
}

vector<int> GetNeighbors(Particle p) {
	vector<int> neighbors;
	glm::vec2 cellID = getCellID(p);
	neighbors.insert(neighbors.end(), spaceMap[cellID].begin(), spaceMap[cellID].end());
	//neighbors.insert(neighbors.end(), spaceMap[cellID - 1].begin(), spaceMap[cellID - 1].end());
	//neighbors.insert(neighbors.end(), spaceMap[cellID + 1].begin(), spaceMap[cellID + 1].end());
	//neighbors.insert(neighbors.end(), spaceMap[cellID - NUM_CELL_X].begin(), spaceMap[cellID - NUM_CELL_X].end());
	//neighbors.insert(neighbors.end(), spaceMap[cellID - NUM_CELL_X-1].begin(), spaceMap[cellID - NUM_CELL_X-1].end());
	//neighbors.insert(neighbors.end(), spaceMap[cellID - NUM_CELL_X + 1].begin(), spaceMap[cellID - NUM_CELL_X + 1].end());
	//neighbors.insert(neighbors.end(), spaceMap[cellID + NUM_CELL_X].begin(), spaceMap[cellID + NUM_CELL_X].end());
	//neighbors.insert(neighbors.end(), spaceMap[cellID + NUM_CELL_X + 1].begin(), spaceMap[cellID + NUM_CELL_X + 1].end());
	//neighbors.insert(neighbors.end(), spaceMap[cellID + NUM_CELL_X -1].begin(), spaceMap[cellID + NUM_CELL_X - 1].end());

	return neighbors;
}

void CollisionCheck(vector<Particle> &particles, int pID) {
	vector<int> neighbors = GetNeighbors(particles[pID]);
	for (int j = 0; j < neighbors.size(); j++) {
		glm::vec2 axis = {particles[pID].curr - particles[neighbors[j]].curr};
		GLfloat length = sqrt(axis.x * axis.x + axis.y * axis.y);
		glm::vec2 norm;
		if (pID != j) {
			if (length < RADIUS * 2) {
				norm = { axis.x / length, axis.y / length };
				GLfloat delta = RADIUS * 2 - length;
				norm = { norm.x * delta * 0.5, norm.y * delta * 0.5 };
				if (length == 0) {
					int angle = rand() % 361;
					norm.x = (cos(angle) * RADIUS);
					norm.y = (sin(angle) * RADIUS);
				}
				glm::vec2 displacement = {particles[pID].curr - particles[pID].prev};
					
				particles[pID].curr.x += norm.x;
				particles[neighbors[j]].curr.x -= norm.x;
				particles[pID].curr.y += norm.y;
				particles[neighbors[j]].curr.y -= norm.y;
			}
		}
	}
}

					/*particles[i].prev.x += displacement.x * 0.1;
					particles[j].prev.x -= displacement.x * 0.1;
					particles[i].prev.y += displacement.y * 0.1;
					particles[j].prev.y -= displacement.y * 0.1;*/


void HandleCollisions(vector<Particle>& particles, int column) {
	for (int i = column; i < NUM_CELL_X; i ++) {
		vector<int> inCellParticles = GetSameCellParticles(glm::vec2(column,i));
		for (int j = 0; j < inCellParticles.size(); j++) {
			CollisionCheck(particles, inCellParticles[j]);
		}
	}
}

void ThreadCollisions(vector<Particle>& particles) {
	vector<thread> threads;
	int column = 0;

	for (int i = 0; i < NUM_CELL_Y; i++) {
		HandleCollisions(particles, column);
		//threads.push_back(thread(CollisionCheck, ref(particles), column));
	}
	for (thread &t : threads) {
		if (t.joinable()) {
			t.join();
		}
		
	}
}

void checkBounds(vector<Particle> &particles) {
	for (int i = 0; i < NUM_CIRCLES; i++) {
		//TODO: replace with verlet integration
		glm::vec2 displacement;
		if (particles[i].curr.y + RADIUS > HEIGHT) {
			displacement = particles[i].curr - particles[i].prev;
			particles[i].curr.y = HEIGHT - RADIUS;
			//particles[i].prev.y = particles[i].curr.y+ displacement.y ;
		}
		if (particles[i].curr.y - RADIUS < 0) {
			displacement = particles[i].curr - particles[i].prev;
			particles[i].curr.y = 1 + RADIUS; 
			//particles[i].prev.y = particles[i].curr.y + displacement.y;
		}

		if (particles[i].curr.x + RADIUS > WIDTH) {
			displacement = particles[i].curr - particles[i].prev;
			particles[i].curr.x = WIDTH - RADIUS;
			//particles[i].prev.x = particles[i].curr.x + displacement.x;

		}
		if (particles[i].curr.x - RADIUS < 0) {
			displacement = particles[i].curr - particles[i].prev;
			particles[i].curr.x = RADIUS;
		//	particles[i].prev.x = particles[i].curr.x + displacement.x;
		}
	}
}

void updatePositions(vector<Particle>& particles) {

	for (int i = 0; i < NUM_CIRCLES; i++) {
		glm::vec2 displacement = particles[i].curr - particles[i].prev;
		particles[i].prev = particles[i].curr;
		//particles[i].acc.y *= 1/NUM_SUBSTEPS;
		particles[i].curr += displacement + particles[i].acc;
		particles[i].acc = {};
	}
}


void Update(vector<Particle>& particles, GLFWwindow* window) {
	divideSpace(particles);
	applyG(particles);
	ThreadCollisions(particles);
	checkBounds(particles);
	updatePositions(particles);
	checkBounds(particles);

	initTime = glfwGetTime();
	frames = 0;
	
	
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
	vector<Particle> particles;
	int n = (SEGMENTS * 3) + 3;
	MakeParticleGrid(particles);
	
	Shader shader("VertexShader", "FragmentShader");
	Shader boundryShader("VertexShader", "FragmentShader");
	
	glfwSwapBuffers(window);

	GLuint* VAO = new GLuint[NUM_CIRCLES];
	GLuint* VBO = new GLuint[NUM_CIRCLES];
	GLuint* EBO = new GLuint[NUM_CIRCLES];
	int size = NUM_CIRCLES;

	for (int i = 0; i < NUM_CIRCLES; i++) {
		GLuint* buffers = CreateBuffers(particles[i].vertices, particles[i].EBOIndices, n);
		VAO[i] = buffers[0], VBO[i] = buffers[1], EBO[i] = buffers[2];
	}
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	int modelLoc = glGetUniformLocation(shader.shaderID, "model");
	int projLoc = glGetUniformLocation(shader.shaderID, "proj");

	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);
	
	initTime = glfwGetTime();
	initTime2 = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		bool nextFrame = processInput(window, particles);
		shader.useShader();
		/*if (NUM_CIRCLES != size) {
			GLuint* temp = new GLuint[NUM_CIRCLES];
			memcpy(temp, VAO, size * sizeof(GLuint));
			delete[] VAO;
			VAO = temp;

			GLuint* temp1 = new GLuint[NUM_CIRCLES];
			memcpy(temp1, VBO, size * sizeof(GLuint));
			delete[] VBO;
			VBO = temp1;

			GLuint* temp2 = new GLuint[NUM_CIRCLES];
			memcpy(temp2, EBO, size * sizeof(GLuint));
			
			delete[] EBO;
			EBO = temp2;
			for (int i = size; i < NUM_CIRCLES; i++) {
				GLuint* buffers = CreateBuffers(particles[i].vertices, particles[i].EBOIndices, n);
				VAO[i] = buffers[0], VBO[i] = buffers[1], EBO[i] = buffers[2];
			}
			size = NUM_CIRCLES;
		}*/

		if (!pause) {
			
			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			for (int i = 0; i < particles.size(); i++) {
				glBindVertexArray(VAO[i]);
				glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(particles[i].curr.x, particles[i].curr.y, 0.0f));

				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

				glBufferData(GL_ARRAY_BUFFER, n * sizeof(GLfloat), particles[i].vertices, GL_STATIC_DRAW);

				glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, 0);
			}
			
			if (nextFrame) {
				frames++;
				frames2++;
				finalTime = glfwGetTime();
				finalTime2 = glfwGetTime();
				if (finalTime - initTime >= TIME_STEP) {
					for (int i = 0; i < NUM_SUBSTEPS; i++) {
						Update(particles, window);
					}
				}
			}
				if (finalTime2 - initTime2 >= 1.0) {
					int fps = (frames2 / (finalTime2 - initTime2))/NUM_SUBSTEPS;
					string f = "FPS: " + to_string(fps);
					const char* str_fps = (f).c_str();
					// I dont think this is correct
					glfwSetWindowTitle(window, str_fps);

					//check particles lost
					particleInvariantCheck(particles);

					frames2 = 0;
					initTime2 = glfwGetTime();
				}
				
			
			//glfwSwapBuffers(window);
		}
		
		
	glfwPollEvents();
	}

	//TODO: RUN VALGRIND
	//TODO: change color with velocity
	//TODO: chemical reactions and stuff
	//TODO: make particles move with window movement
	//TODO: make different containers
	//TODO: make variable size container
	//TODO: make a toggle between 2d and 3d
	for (int i = 0; i < size; i++) {
		glDeleteVertexArrays(1, &VAO[i]);
		glDeleteBuffers(1, &VBO[i]);
		glDeleteBuffers(1, &EBO[i]);
	}
	shader.Delete();
	glfwDestroyWindow(window);
	for (int i = 0; i < NUM_CIRCLES; i++) {
		delete[] particles[i].EBOIndices;
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

