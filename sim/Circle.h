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
	int px; //MAKE THESE INTO VEC2s PROBAB:
	int py;
	int x;
	int y;

};
