#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <windows.h>   // for MS Windows

#include "Circle.h"
#include "Shader.h"

//constants
#define PI 3.1415926535897932384626433
#define SEGMENTS 90 // just dont go over 90 for some reason
#define RADIUS .5 // <= 1
#define WIDTH 1000
#define HEIGHT 1000

//globals
bool pause = false;
double prevTime = 0; // used to make sure button presses work properly
double timeOffset = 0;//used to offset time after pause


const char* fragmentShaderSource = "\n\0";

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

	//generate a circle
	int n = (SEGMENTS * 3) + 3;
	GLfloat* vertices = new GLfloat[n];
	GLuint* EBOIndices = new GLuint[n];
	GenerateCircle(vertices, EBOIndices, n);


	Shader shader("VertexShader", "FragmentShader");
	
	GLuint* buffers = CreateBuffers(vertices, EBOIndices, n);
	GLuint VAO = buffers[0], VBO = buffers[1], EBO = buffers[2];
	
	glfwSwapBuffers(window);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		if (!pause) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader.useShader();
			//initialize our space matrices 
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 proj = glm::mat4(1.0f);
			//move the whole world around the camera rather than
			//moving the camera around the world. so lets just move the world
			//back a little bit
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
			//now lets create the perspective. 
			//even though im working in 2d, this just makes things easier
			//to work with as i have a larger range of numbers to work with
			//than just the clip space.
			//45 is standard
			//0.1 clip close is standard
			proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
			model = glm::rotate(model, (float)(glfwGetTime()), glm::vec3(1.0f, 1.0f, 0.0f));
			//send the information to the shader
			int modelLoc = glGetUniformLocation(shader.shaderID, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			int viewLoc = glGetUniformLocation(shader.shaderID, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			int projLoc = glGetUniformLocation(shader.shaderID, "proj");
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

			glBindVertexArray(VAO);

			glBufferData(GL_ARRAY_BUFFER, n * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

			glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, 0);
			glfwSwapBuffers(window);
		}
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	shader.Delete();
	glfwDestroyWindow(window);
	delete[] EBOIndices;
	delete[] vertices;
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

