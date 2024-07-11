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
//#include <gl/glut.h>


#include "Circle.h"
#include "Shader.h"
#include "Boundry.h"

//constants
#define PI 3.1415926535897932384626433
#define SEGMENTS 25 // just dont go over 90 for some reason
#define RADIUS 10 // <= 1
#define WIDTH 1000
#define HEIGHT 1000
#define NUM_CIRCLES 200
#define TARGET_FPS 60
#define G 0

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


void processInput(GLFWwindow* window) {
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
}
void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
	glViewport(0, 0, w, h);
}

void GenerateCircle(GLfloat*& vertices, GLuint*& EBOIndices, int n) {
	vertices[0] = 0.0;
	vertices[1] = 0.0;
	vertices[2] = 0.0;
	int pos = 2;
	int c = 0;
	GLfloat degreeIncrement = glm::radians((float)(360 / SEGMENTS));
	while (pos < n - 1) {
		GLfloat currentDegree = c * degreeIncrement;
		pos++;
		vertices[pos] = (GLfloat)(RADIUS * cos(currentDegree));
		pos++;
		vertices[pos] = (GLfloat)(RADIUS * sin(currentDegree));
		pos++;
		vertices[pos] = (GLfloat)0.0;
		c++;
	}
	int i = 0;
	int curr = 1;
	while (i < n - 6) {
		EBOIndices[i] = 0;
		i++;
		EBOIndices[i] = curr;
		i++;
		curr++;
		EBOIndices[i] = curr;
		i++;
	}
	EBOIndices[i] = 0;
	i++;
	EBOIndices[i] = 1;
	i++;
	EBOIndices[i] = curr;

}
//this is fine for my purpose currently. but it might be problematic later with different sized shit
GLuint* CreateBuffers(GLfloat* &vertices, GLuint*& EBOIndices, int n) {
	GLuint* buffers = new GLuint[3];
	GLuint VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, n * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, n * sizeof(GLuint), EBOIndices, GL_STATIC_DRAW);
	glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	buffers[0] = VAO;
	buffers[1] = VBO;
	buffers[2] = EBO;
	return buffers;
}

void MakeCircleGrid(vector<Circle>& circles) {
	int n = (SEGMENTS * 3) + 3;
	int j = 0;
	int i = 0;
	int counter = 0;

	while (counter < NUM_CIRCLES) {
		int max = (WIDTH-RADIUS) / ((RADIUS * 3));
		//cout << max << endl;
		if (i % max == 0) { //calculate the 50 to be less than the width
			j++;
			i = 0;
		}
		Circle c;
		GLfloat* vertices = new GLfloat[n];
		GLuint* EBOIndices = new GLuint[n];
		GenerateCircle(vertices, EBOIndices, n);
		c.EBOIndices = EBOIndices;
		c.vertices = vertices;
		c.size = n;
		c.x = RADIUS* 2 + i * RADIUS * 3;
		c.y = RADIUS + j * RADIUS * 3;
		c.xVel = (rand() % 10) * pow(-1, rand() % 2 + 1);
		c.yVel = (rand() % 20);
		circles.push_back(c);
		i++;
		counter++;
	}
}

void Update(vector<Circle>& circles) {
	frames++;
	frames2++;
	finalTime = glfwGetTime();
	finalTime2 = glfwGetTime();
	//TODO: make something that prints to the screen instead of cout
	//handle collisions and stuff
	if (finalTime2 - initTime2 >= 1.0) {
		int fps = frames2 / (finalTime2 - initTime2);
		if (fps < TARGET_FPS) {
			cout << "LOW FRAME RATE DETECTED: " << fps << endl;
		}
		frames2 = 0;
		initTime2 = glfwGetTime();
	}
	float timeStep = (1.0 / 60);
	if (finalTime - initTime >= timeStep) { 
		for (int i = 0; i < NUM_CIRCLES; i++) {
			if (circles[i].y + RADIUS <= HEIGHT && circles[i].y - RADIUS >= 0) {
				circles[i].yVel += G * timeStep; //technically a little off I hope it doesnt cause any issues
				circles[i].y += circles[i].yVel;
			}
			if (circles[i].x + RADIUS <= WIDTH && circles[i].x - RADIUS >= 0) {
				//circles[i].xVel += G * timeStep; //technically a little off I hope it doesnt cause any issues
				circles[i].x += circles[i].xVel;
			}
			for (int j = 0; j < NUM_CIRCLES; j++) {
				if (i !=j && fabs(circles[i].y - circles[j].y) < RADIUS * 2 && fabs(circles[i].x - circles[j].x) < RADIUS * 2) {
					cout << "collision" << endl;
					circles[i].xVel *= -1;//dampening;
					circles[j].xVel *= -1; //dampening;
					circles[i].yVel *= -1;// dampening;
					circles[j].yVel *= -1;// dampening;
					glm::vec2 C_diff = { circles[i].x - circles[j].x,circles[i].y - circles[j].y };
					glm::vec2 v_diff = { circles[i].xVel - circles[j].xVel,circles[i].yVel - circles[j].yVel };
					float dotProduct = glm::dot(v_diff, C_diff);
					float normSquared = glm::dot(C_diff, C_diff);
					glm::vec2 correction = 2.0f * (dotProduct / normSquared) * C_diff;
					circles[i].x -= correction.x;
					circles[i].y -= correction.y;

					C_diff = { circles[j].x - circles[i].x,circles[j].y - circles[i].y };
					v_diff = { circles[j].xVel - circles[i].xVel,circles[j].yVel - circles[i].yVel };
					dotProduct = glm::dot(v_diff, C_diff);
					normSquared = glm::dot(C_diff, C_diff);
					correction = 2.0f * (dotProduct / normSquared) * C_diff;
					circles[j].x -= correction.x;
					circles[j].y -= correction.y;
					
										
				}
			}
		}

		for (int i = 0; i < NUM_CIRCLES; i++) {
			//TODO: replace with verlet integration
			
			if (circles[i].y + RADIUS >= HEIGHT) { 
				circles[i].y = HEIGHT - RADIUS;
				circles[i].yVel *= -dampening;
				if (fabs(circles[i].yVel) < 1) {
					circles[i].yVel = 0.0;
				}
			}else if (circles[i].y - RADIUS <= 0) { 
				circles[i].y = 1 + RADIUS; //why do I have to add 1 here;
				circles[i].yVel *= dampening;
				cout << circles[i].yVel << endl;
			}
			
			if (circles[i].x + RADIUS >= WIDTH) { 
				circles[i].x = WIDTH - RADIUS;
				circles[i].xVel *= -dampening;
				if (fabs(circles[i].xVel) < 1) {
					circles[i].xVel = 0.0;
				}
			}
			else if (circles[i].x - RADIUS <= 0) { 
				circles[i].x = RADIUS;
				circles[i].xVel *= -dampening;
				if (fabs(circles[i].xVel) < 1) {
					circles[i].xVel = 0.0;
				}
			}
		}
		initTime = glfwGetTime();
		frames = 0;
	}
}

void BeginSim() {
	//create window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Circle", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Boundry boundry(5, 5);//actually make the length and width do something cuz rn it does nothing
	boundry.createBoundry();

	//TODO: make a mesh instead the size of the circles doesnt change with the 
	// size of the window
	//generate circles
	vector<Circle> circles;
	int n = (SEGMENTS * 3) + 3;
	MakeCircleGrid(circles);
	
	Shader shader("VertexShader", "FragmentShader");
	Shader boundryShader("VertexShader", "FragmentShader");
	
	glfwSwapBuffers(window);

	GLuint* VAO = new GLuint[NUM_CIRCLES];
	GLuint* VBO = new GLuint[NUM_CIRCLES];
	GLuint* EBO = new GLuint[NUM_CIRCLES];

	for (int i = 0; i < NUM_CIRCLES; i++) {
		GLuint* buffers = CreateBuffers(circles[i].vertices, circles[i].EBOIndices, n);
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
		processInput(window);
		shader.useShader();
		if (!pause) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			for (int i = 0; i < NUM_CIRCLES; i++) {
				glBindVertexArray(VAO[i]);
				glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(circles[i].x, circles[i].y, 0.0f));

				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

				glBufferData(GL_ARRAY_BUFFER, n * sizeof(GLfloat), circles[i].vertices, GL_STATIC_DRAW);

				glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, 0);
			}
			// distance check
			//TODO: prob put this in utils file or smth
			
			
			
			glfwSwapBuffers(window);
			Update(circles);
		}
		
	glfwPollEvents();
	}

	//TODO: RUN VALGRIND
	for (int i = 0; i < NUM_CIRCLES; i++) {
		glDeleteVertexArrays(1, &VAO[i]);
		glDeleteBuffers(1, &VBO[i]);
		glDeleteBuffers(1, &EBO[i]);
	}
	shader.Delete();
	glfwDestroyWindow(window);
	for (int i = 0; i < NUM_CIRCLES; i++) {
		delete[] circles[i].EBOIndices;
		delete[] circles[i].vertices;
	}
	glfwTerminate();
	return;
}

int main() {
	//Initialization
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	prevTime = glfwGetTime();
	BeginSim();
	return 0;
}

