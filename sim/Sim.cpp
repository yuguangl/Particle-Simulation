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
#define RADIUS 30 // <= 1
#define WIDTH 1000
#define HEIGHT 1000
#define NUM_CIRCLES 20
#define TARGET_FPS 60
#define G 100

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
float TIME_STEP = (1.0 / 60);


bool processInput(GLFWwindow* window, const vector<Circle> circles) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (GetAsyncKeyState(' ') && glfwGetTime() - prevTime > 0.2) {
			for (int i = 0; i < NUM_CIRCLES; i++) {
				cout << "circle[" << i << "]: (" << circles[i].x << ", " << circles[i].y << ")" << endl;
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
		int max = (WIDTH-RADIUS-200) / ((RADIUS * 3));
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
		
		c.x = 100 + RADIUS* 2 + i * RADIUS * 3;
		c.y = 100 + RADIUS + j * RADIUS * 3;
		c.xVel = rand() % 10 * pow(-1, rand() * 2 % 1);
		c.yVel = rand() % 10 * pow(-1, rand() * 2 % 1);
		c.py = c.y *0.9;
		c.px = -10 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (c.x - c.x-10))) * pow(-1, rand()* 2 % 1);
		c.acc.x = 0xb;
		c.acc.y = 0;
		circles.push_back(c);
		i++;
		counter++;
	}

}
bool withinBounds(const Circle c) {
	if (c.x < 0 || c.x > WIDTH || c.y < 0 || c.y > HEIGHT) {
		return false;
	}
	return true;
}

void Update(vector<Circle>& circles, GLFWwindow* window) {
	frames++;
	frames2++;
	finalTime = glfwGetTime();
	finalTime2 = glfwGetTime();
	if (finalTime2 - initTime2 >= 1.0) {
		int fps = frames2 / (finalTime2 - initTime2);
		string f = "FPS: " + to_string(fps);
		const char* str_fps = (f).c_str();
		glfwSetWindowTitle(window, str_fps);
		if (fps < TARGET_FPS) {
			cout << "LOW FRAME RATE DETECTED: " << fps << endl;
		}

		int counter = 0;
		for (int i = 0; i < NUM_CIRCLES; i++) {
			if (withinBounds(circles[i])) {
				counter++;
			}
		}
		if (counter != NUM_CIRCLES) {
			cout << "Circles: " << counter << endl;
		}

		frames2 = 0;
		initTime2 = glfwGetTime();
	}
	GLfloat timeStep = TIME_STEP;
	if (finalTime - initTime >= timeStep) { 
		for (int i = 0; i < NUM_CIRCLES; i++) {
			circles[i].acc.y = G;
			for (int j = 0; j < NUM_CIRCLES; j++) {
				glm::vec2 axis = { circles[i].x - circles[j].x, circles[i].y - circles[j].y };
				GLfloat length = sqrt(axis.x * axis.x + axis.y * axis.y);
				glm::vec2 norm;
				if (i !=j) {
					if (length < RADIUS * 2) {
						norm = { axis.x / length, axis.y / length };
						GLfloat delta = RADIUS  * 2 - length;
						norm = { norm.x * delta * 0.5, norm.y * delta * 0.5 };
						if (length == 0) {
							int angle = rand() % 361;
							norm.x = cos(angle) * RADIUS ;
							norm.y = sin(angle) * RADIUS ;
						}
						
						circles[i].x += norm.x ;
						circles[j].x -= norm.x ;
						circles[i].y += norm.y ;
						circles[j].y -= norm.y ;
						//circles[j].acc -= 1;
						//circles[i].acc.x += 1;
						/*circles[i].yVel *= dampening;
						circles[j].yVel *= dampening;*/

						//check bounds  and collision directions
						//if (circles[i].x <= RADIUS) {
						//	circles[j].x -= 2 * norm.x;
						//	//circles[j].x -= 2 * norm.x;
						//	//circles[j].px = circles[j].x - 10;
						//}
						//else if (circles[i].x >= WIDTH - RADIUS) {
						//	circles[j].x -= 2 * norm.x;
						//	//circles[j].x -= 2 * norm.x;
						//	//circles[j].px = circles[j].x + 20;
						//}
						//else if (circles[j].x <= RADIUS) {
						//	circles[i].x += 2* norm.x;
						//	//circles[i].px += 2 * norm.x;
						//	//circles[i].px = circles[i].x - 10;
						//}
						//else if (circles[j].x >= WIDTH - RADIUS) {
						//	circles[i].x += 2 * norm.x;
						//	//circles[i].px += 2 * norm.x;
						//	//circles[i].px = circles[i].x + 20;
						//}
						//else {//no walls
						//	circles[i].x += norm.x;
						//	circles[j].x -= norm.x;
						//	//circles[j].px = circles[j].x;
						//	//circles[i].px = circles[i].x;
						//	
						//}
						//
						//if (circles[i].y <= RADIUS || circles[i].y >= HEIGHT - RADIUS) {
						//	circles[j].y -= 2 * norm.y;
						//	//circles[j].py -= 2 * norm.y;
						//	//circles[j].py = circles[j].y;
						//}
						//else if (circles[j].y <= RADIUS || circles[j].y >= HEIGHT - RADIUS) {
						//	circles[i].y += 2 * norm.y;
						//	//circles[i].py += 2 * norm.y;
						//	//circles[i].py = circles[i].y;
						//}
						//else {
						//	circles[i].y += norm.y;
						//	circles[j].y -= norm.y;
						//	//circles[i].py = circles[i].y;
						//	//circles[j].py = circles[j].y;
						//}
					}
				}
			}
		}

		for (int i = 0; i < NUM_CIRCLES; i++) {
			glm::vec2 displacement = {circles[i].x - circles[i].px , circles[i].y - circles[i].py };
			circles[i].px = circles[i].x;
			circles[i].py = circles[i].y;
			
			circles[i].acc.y *= timeStep;
			circles[i].x += displacement.x;
			circles[i].y += displacement.y;
			circles[i].x += circles[i].acc.x;
			circles[i].y += circles[i].acc.y;
			circles[i].acc.x = 0.0;
			circles[i].acc.y = 0.0;
		}
		

		initTime = glfwGetTime();
		frames = 0;

	}
	for (int i = 0; i < NUM_CIRCLES; i++) {
		//TODO: replace with verlet integration
		glm::vec2 displacement;
		if (circles[i].y + RADIUS >= HEIGHT) {
			displacement = { circles[i].x - circles[i].px , circles[i].y - circles[i].py };
			circles[i].y = HEIGHT - RADIUS;
			//circles[i].py = (circles[i].y + displacement[1] * dampening); //DO I CHANGE THIS BACK TO * 0.9??
			//circles[i].yVel *= -dampening;
		}
		if (circles[i].y - RADIUS <= 0) {
			displacement = { circles[i].x - circles[i].px , circles[i].y - circles[i].py };
			circles[i].y = 1 + RADIUS; //why do I have to add 1 here;
			//circles[i].py = circles[i].y + displacement[1] * dampening;
			//circles[i].yVel *= -dampening;
		}

		if (circles[i].x + RADIUS >= WIDTH) {
			displacement = { circles[i].x - circles[i].px , circles[i].y - circles[i].py };
			circles[i].x = WIDTH - RADIUS;
			//circles[i].px = circles[i].x + displacement[0] * dampening;
			//circles[i].xVel *= -dampening;

		}
		if (circles[i].x - RADIUS <= 0) {
			displacement = { circles[i].x - circles[i].px , circles[i].y - circles[i].py };
			circles[i].x = RADIUS;
			//circles[i].px = circles[i].x + displacement[0] * dampening;
			//circles[i].xVel *= -dampening;
		}
	}
}



void BeginSim() {
	//create window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Starting Simulation...", NULL, NULL);
	
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
		bool nextFrame = processInput(window, circles);
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
			if (nextFrame) {
				for (int i = 0; i < 2; i++) {
					Update(circles, window);
				}

			}
			
			
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
	srand(static_cast <unsigned> (time(0)));
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	prevTime = glfwGetTime();
	BeginSim();
	return 0;
}

