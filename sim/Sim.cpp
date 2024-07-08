#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include "Circle.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

#define PI 3.1415926535897932384626433
#define SEGMENTS 90 // just dont go over 90 for some reason
#define RADIUS .75 // <= 1
#define G 9.81
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(0.7f, 0.0f, 1.0f, 1.0f);\n"
"}\n\0";

void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
	glViewport(0, 0, w, h);
}

vector<pair<GLfloat, GLfloat>> generateVertices(float r, int n) {
	vector<pair<GLfloat, GLfloat>> circlePoints(n, {0,0});
	GLfloat degreeIncrement = (360 / n) * PI/180;

	for (int i = 0; i < n; i++) {
		GLfloat currentDegree = i * degreeIncrement;
		circlePoints[i].first = r * cos(currentDegree);
		circlePoints[i].second = r * sin(currentDegree);
	}
	return circlePoints;
}

vector<Circle> generateCircles(float r, float sep, int count) {
	//TODO: try generating in a grid pattern
	//use a mesh instead?
	// learn how to commit this shit
	//if repetative math is a thing then use a new file that will 
	// have a void function to call that will do that math for you
	vector<pair<GLfloat, GLfloat>> circlePoints = generateVertices(r, SEGMENTS);

	float offset = ((count * r) + ((count - 1) * sep))/2 + r/2;
	int EBOSize = (circlePoints.size() * 3 * count) + (3 * count);
	int EBOi = 0;
	int EBOcurr = 1;
	Circle c;
	vector<Circle> circles(count, c);
	for (int i = 0; i < count; i++) {
		float adjustedOffset = offset + (i * (r + sep));
		int n = (circlePoints.size() * 3) + 3;
		c.vertices = new GLfloat[n];
		c.vertices[0] = 0;
		c.vertices[1] = 0;
		c.vertices[2] = 0;
		int pos = 2;
		for (int i = 0; i < circlePoints.size(); i++) {
			pos++;
			c.vertices[pos] = circlePoints[i].first ;
			pos++;
			c.vertices[pos] = circlePoints[i].second;
			pos++;
			c.vertices[pos] = 0.0f;
		}
		
		GLuint* EBOIndices = new GLuint[EBOSize];
		
		int recent[3] = { EBOi+ 3,EBOi + 4,EBOi+5 };
		while (i < EBOSize - 6) {
			EBOIndices[i] = 0;
			i++;
			EBOIndices[i] = EBOcurr;
			i++;
			EBOcurr++;
			EBOIndices[i] = EBOcurr;
			i++;
		}
		EBOIndices[i] = 0;
		i++;
		EBOIndices[i] = 1;
		i++;
		EBOIndices[i] = EBOcurr;
		i++;

	}

}
void createCircle() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1000, 1000, "Circle", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, 1000, 1000);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	vector<Circle> circles = generateCircles(RADIUS, 0.1, 2);
/*
	vector<pair<GLfloat, GLfloat>> circlePoints = generateVertices(RADIUS, SEGMENTS);
	

	int n = (circlePoints.size() * 3) + 3;
	GLfloat* vertices = new GLfloat[n];

	vertices[0] = 0;
	vertices[1] = 0;
	vertices[2] = 0;
	int pos = 2;
	for (int i = 0; i < circlePoints.size(); i++) {
		pos++;
		vertices[pos] = circlePoints[i].first;
		pos++;
		vertices[pos] = circlePoints[i].second;
		pos++;
		vertices[pos] = 0.0f;
	}
	
	int EBOSize = (circlePoints.size()*3)+3;
	GLuint* EBOIndices = new GLuint[EBOSize];
	int i = 0;
	int curr = 1;
	int recent[3] = { 3,4,5 };
	while (i < EBOSize-6) {
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

	Circle c;
	c.vertices = vertices;
	*/

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

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

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	bool down = true;
	int dampenCount = 0;
	float dampening = 0.05;

	while (!glfwWindowShouldClose(window)) {
		double time = glfwGetTime();
		cout << time << endl;
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		
		//apply gravity
		if (c.vertices[1] - RADIUS > -1) {
			int i = 0;
			while (i < n) {
				i++;
				c.vertices[i] -= 0.001;
				i++;
				i++;
			}
		}
		
		glBufferData(GL_ARRAY_BUFFER, n * sizeof(GLfloat), vertices, GL_STATIC_DRAW);


		glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteShader(shaderProgram);
	glfwDestroyWindow(window);
	glfwTerminate();
	delete[] EBOIndices;
	delete[] vertices;

	return;

}

int main() {
	createCircle();
	return 0;
}

