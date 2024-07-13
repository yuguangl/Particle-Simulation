#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include "Variables.h"
#include "Utils.h"
using namespace std;
using namespace glm;




struct Particle {
	int size;
	GLfloat* vertices;
	GLuint* EBOIndices;
	vec2 acc;
	vec2 prev;
	vec2 curr;
};



float SmoothingKernel(float dist) {
	
	float volume = PI * pow(SMOOTHING_RADIUS, 4) / 6.0f;
	if (0 > SMOOTHING_RADIUS - dist) { return 0; }
	return (pow(SMOOTHING_RADIUS - dist, 3)) / volume;
}

float CalculateDensity(vector<Particle>& particles, int pIndex) {
	float density = 0;
	
	for (int i = 0; i < particles.size(); i++) {
		if (i != pIndex) {
			float dist = GetDistance(particles[i].curr, particles[pIndex].curr);
			//cout << dist << endl;
			float kernelForce = SmoothingKernel(dist);
			density += MASS * kernelForce;
			
		}
			
		
	}
	return density;
	
}

float SmoothingSlope(float dist) {
	if (0 > SMOOTHING_RADIUS - dist) { return 0; }
	return (dist - SMOOTHING_RADIUS)* (12.0f / pow(SMOOTHING_RADIUS, 4) * PI);
}

float DensityToPressure(float density) { // look into this?
	return (density - targetDensity) * PRESSUREC;
}



int randSign() {
	return pow(-1, rand() % 2);
}

vec2 CalculateForce(vector<Particle>& particles, int pIndex) { //calc property
	vec2 pressure = {};
	for (int i = 0; i < particles.size(); i++) {
		if (i != pIndex) {
			vec2 direction;
			float dist = GetDistance(particles[i].curr, particles[pIndex].curr);
			if (dist == 0.0f) {
				direction = { randSign(), randSign() };
			}
			else {
				direction = (particles[i].curr - particles[pIndex].curr) / dist;
			}
			
			float slope = SmoothingSlope(dist);
			float density = densities[i];
			cout << density << endl;
			if (density != 0) {
				pressure += -DensityToPressure(density) * slope * direction * (float)MASS;
			}
			
		}
		

	}
	return pressure;
}

void GenerateParticle(GLfloat*& vertices, GLuint*& EBOIndices, int n) {
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

void MakeParticleGrid(vector<Particle>& particles) {
	int n = (SEGMENTS * 3) + 3;
	int j = 0;
	int i = 0;
	int counter = 0;

	while (counter < NUM_CIRCLES) {
		int max = (WIDTH - RADIUS) / ((RADIUS * 2.5));
		if (i % max == 0) {
			j++;
			i = 0;
		}
		Particle c;
		GLfloat* vertices = new GLfloat[n];
		GLuint* EBOIndices = new GLuint[n];
		GenerateParticle(vertices, EBOIndices, n);
		c.EBOIndices = EBOIndices;
		c.vertices = vertices;
		c.size = n;
		c.curr.x = RADIUS * 2 + i * RADIUS * 2.5;
		c.curr.y = 400 + RADIUS + j * RADIUS * 2.5;
		c.prev.y = c.curr.y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10))) * pow(-1, rand() % 2);;
		c.prev.x = c.curr.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10)))* pow(-1, rand() % 2);
		c.acc.x = 0;
		c.acc.y = 0;
		particles.push_back(c);
		i++;
		counter++;
	}
}

GLuint* CreateBuffers(GLfloat*& vertices, GLuint*& EBOIndices, int n) {
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
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	buffers[0] = VAO;
	buffers[1] = VBO;
	buffers[2] = EBO;
	return buffers;
}
