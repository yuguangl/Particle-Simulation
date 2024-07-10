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
//#include <gl/glut.h>


#include "Circle.h"
#include "Shader.h"
#include "Boundry.h"

//constants
#define PI 3.1415926535897932384626433
#define SEGMENTS 25 // just dont go over 90 for some reason
#define RADIUS 30 // <= 1
#define WIDTH 1500
#define HEIGHT 1500
#define NUM_CIRCLES 10
#define FPS 60

//globals
bool pause = false; //obvious
double prevTime = 0; // used to make sure button presses work properly
double timeOffset = 0;//used to offset time after pause
int frameRate = 0;
double finalTime = 0; //used for frameRate control

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
			cout << pause << endl;
		
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

void BeginSim() {
	//create window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Circle", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Boundry boundry(5, 5);//actually make the length and width do something cuz rn it does nothing
	boundry.createBoundry();

	//generate circles
	vector<Circle> circles;
	int n = (SEGMENTS * 3) + 3;
	for (int i = 0; i < NUM_CIRCLES; i++) {
		Circle c;
		GLfloat* vertices = new GLfloat[n];
		GLuint* EBOIndices = new GLuint[n]; 
		GenerateCircle(vertices, EBOIndices, n);
		c.EBOIndices = EBOIndices;
		c.vertices = vertices;
		c.size = n;
		c.x = 100 + i * RADIUS * 3;
		c.y = 100;
		circles.push_back(c);

	}
	
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

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	int modelLoc = glGetUniformLocation(shader.shaderID, "model");
	int projLoc = glGetUniformLocation(shader.shaderID, "proj");

	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::ortho(0.0f, 1500.0f, 1500.0f, 0.0f, -1.0f, 1.0f);
	
	double initTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		//cout << frameRate / (finalTime - initTime) << endl;
		//frameRate = 0;
		processInput(window);
		shader.useShader();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//make force apply with time
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
			
			glfwSwapBuffers(window);
		frameRate++;
		finalTime = glfwGetTime();

		//NOTE:
		if (finalTime - initTime >= (1.0/60)) {
			cout << frameRate / (finalTime - initTime) << endl;
			initTime = glfwGetTime();
			frameRate = 0;
			for (int i = 0; i < NUM_CIRCLES; i++) {
				circles[i].y += 1;
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
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	prevTime = glfwGetTime();
	
	BeginSim();
	return 0;
}

