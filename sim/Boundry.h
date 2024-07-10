#ifndef BOUNDRY_H
#define BOUNRY_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<iostream>
#include<vector>

class Boundry {
public:

	float width;
	float height;
	GLfloat vertices[48] = {
		-5.1f, 5.0f, 0.0f,
		-5.1,-5.0, 0.0f,
		-5.0,-5.0, 0.0f,
		-5.0,5.0, 0.0f,

		-5.1,-5.0, 0.0f,
		-5.1,-5.1, 0.0f,
		5.1,-5.0, 0.0f,
		5.1,-5.1, 0.0f,

		-5.1, 5.1, 0.0f,
		-5.1, 5.0, 0.0f,
		5.1, 5.1, 0.0f,
		5.1, 5.0, 0.0f,

		5.0, 5.0, 0.0f,
		5.1, 5.0, 0.0f,
		5.0,-5.0, 0.0f,
		5.1, -5.0, 0.0f,
	};

	GLuint indices[24] = {
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 5,

		8, 9, 10,
		10, 11, 9,

		12, 13, 14,
		14, 15, 13
	};

	GLuint VAO, VBO, EBO;

	Boundry(float width, float height);

	void createBoundry();
};

#endif
