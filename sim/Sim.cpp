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
#include "Constants.h"


#include "Particle.h"
#include "Shader.h"
#include "Boundry.h"



//globals
bool pause = false; //obvious
double prevTime = 0; // used to make sure button presses work properly
double timeOffset = 0;//used to offset time after pause
int frames = 0;
double finalTime = 0; //used for fps control
double initTime = 0; //use for fps control
double finalTime2 = 0;
double initTime2 = 0;
double realTime; //for testing purposes
int frames2 = 0;
float dampening = 0.5;
int c = 0;

bool processInput(GLFWwindow* window, const vector<Particle> particles) {
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

void particleInvariantCheck(const vector<Particle> particles){
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
		particles[i].acc.y = G;
	}
}

void collisionCheck(vector<Particle> &particles) {
	for (int i = 0; i < NUM_CIRCLES; i++) {
		for (int j = 0; j < NUM_CIRCLES; j++) {
			glm::vec2 axis = { particles[i].curr.x - particles[j].curr.x, particles[i].curr.y - particles[j].curr.y };
			GLfloat length = sqrt(axis.x * axis.x + axis.y * axis.y);
			glm::vec2 norm;
			if (i != j) {
				if (length < RADIUS * 2) {
					norm = { axis.x / length, axis.y / length };
					GLfloat delta = RADIUS * 2 - length;
					norm = { norm.x * delta * 0.5, norm.y * delta * 0.5 };
					if (length == 0) {
						int angle = rand() % 361;
						norm.x = cos(angle) * RADIUS;
						norm.y = sin(angle) * RADIUS;
					}

					particles[i].curr.x += norm.x;
					particles[j].curr.x -= norm.x;
					particles[i].curr.y += norm.y;
					particles[j].curr.y -= norm.y;
				}
			}
		}
	}
}

void updatePositions(vector<Particle>& particles) {

	for (int i = 0; i < NUM_CIRCLES; i++) {
		glm::vec2 displacement = { particles[i].curr.x - particles[i].prev.x , particles[i].curr.y - particles[i].prev.y };
		particles[i].prev = particles[i].curr;
		particles[i].acc.y *= TIME_STEP * 0.5;
		particles[i].curr += displacement + particles[i].acc;
		particles[i].acc = {0,0};
	}
}

void checkBounds(vector<Particle> &particles) {
	for (int i = 0; i < NUM_CIRCLES; i++) {
		//TODO: replace with verlet integration
		glm::vec2 displacement;
		if (particles[i].curr.y + RADIUS >= HEIGHT) {
			displacement = { particles[i].curr.x - particles[i].prev.x , particles[i].curr.y - particles[i].prev.y };
			particles[i].curr.y = HEIGHT - RADIUS;
		}
		if (particles[i].curr.y - RADIUS <= 0) {
			displacement = { particles[i].curr.x - particles[i].prev.x , particles[i].curr.y - particles[i].prev.y };
			particles[i].curr.y = 1 + RADIUS; 
		}

		if (particles[i].curr.x + RADIUS >= WIDTH) {
			displacement = { particles[i].curr.x - particles[i].prev.x , particles[i].curr.y - particles[i].prev.y };
			particles[i].curr.x = WIDTH - RADIUS;

		}
		if (particles[i].curr.x - RADIUS <= 0) {
			displacement = { particles[i].curr.x - particles[i].prev.x , particles[i].curr.y - particles[i].prev.y };
			particles[i].curr.x = RADIUS;
		}
	}
}

void Update(vector<Particle>& particles, GLFWwindow* window) {
	//check fps
	if (finalTime2 - initTime2 >= 1.0) {
		int fps = frames2 / (finalTime2 - initTime2);
		string f = "FPS: " + to_string(fps);
		const char* str_fps = (f).c_str();
		// I dont think this is correct
		glfwSetWindowTitle(window, str_fps);

		//check particles lost
		particleInvariantCheck(particles);

		frames2 = 0;
		initTime2 = glfwGetTime();
	}
	if (finalTime - initTime >= TIME_STEP/8) { 
		applyG(particles);
		collisionCheck(particles);
		updatePositions(particles);
		checkBounds(particles);
		initTime = glfwGetTime();
		frames = 0;
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
	vector<Particle> particles;
	int n = (SEGMENTS * 3) + 3;
	MakeParticleGrid(particles);
	
	Shader shader("VertexShader", "FragmentShader");
	Shader boundryShader("VertexShader", "FragmentShader");
	
	glfwSwapBuffers(window);

	GLuint* VAO = new GLuint[NUM_CIRCLES];
	GLuint* VBO = new GLuint[NUM_CIRCLES];
	GLuint* EBO = new GLuint[NUM_CIRCLES];

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
		if (!pause) {
			
			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			for (int i = 0; i < NUM_CIRCLES; i++) {
				glBindVertexArray(VAO[i]);
				glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(particles[i].curr.x, particles[i].curr.y, 0.0f));

				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

				glBufferData(GL_ARRAY_BUFFER, n * sizeof(GLfloat), particles[i].vertices, GL_STATIC_DRAW);

				glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, 0);
			}
			
			glfwSwapBuffers(window);
			if (nextFrame) {
				frames++;
				frames2++;
				for (int i = 0; i < 8; i++) {
					finalTime = glfwGetTime();
					finalTime2 = glfwGetTime();
					Update(particles, window);
				}
			}
		}
		
		
	glfwPollEvents();
	}

	//TODO: RUN VALGRIND
	//TODO: change color with velocity
	//TODO: chemical reactions and stuff
	for (int i = 0; i < NUM_CIRCLES; i++) {
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

