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
	float xAcc;
	float yAcc;
	int x;
	int y;

};
