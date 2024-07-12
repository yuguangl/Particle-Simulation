#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
using namespace std;

struct Circle {
	int size;
	GLfloat* vertices;
	GLuint* EBOIndices;
	glm::vec2 acc;
	GLint px; //MAKE THESE INTO VEC2s PROBAB:
	GLint py;
	GLint x;
	GLint y;
	GLfloat xVel;
	GLfloat yVel;
};
