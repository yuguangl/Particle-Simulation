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
	int size; //uh I should probably get rid of this?
	vec2 acc;
	vec2 prev;
	vec2 curr;
	vec2 press;

};





float NearKernel(float dist) {
	float delta = SMOOTHING_RADIUS - dist;
	return (float)(pow(delta, 3) * (10.0f / (PI * pow(SMOOTHING_RADIUS, 5))));
}

//vec2 CalculateDensity(vector<Particle>& particles, int pIndex) {
//	float density = 0.0f;
//	float nearDensity = 0.0f;
//	for (int i = 0; i < particles.size(); i++) {
//		if (i != pIndex) {
//			float dist = GetDistance(particles[i].curr, particles[pIndex].curr);
//			//cout << dist << endl;
//			if (dist <= SMOOTHING_RADIUS) {
//				density += MASS * DensityKernel(dist);
//				nearDensity += MASS * NearKernel(dist);
//			}
//			
//			
//		}
//			
//		
//	}
//	return vec2{density, nearDensity};
//	
//}



float NearSmoothingSlope(float dist) {
	float delta = SMOOTHING_RADIUS - dist;
	return (float)(-pow(delta, 2) * 30.0f / (pow(SMOOTHING_RADIUS, 5) * PI));
}

float DensityToPressure(float density) { // look into this?
	return (density - targetDensity) * PRESSUREC;
}

float ReturnPressure(float d1, float d2) {
	return (DensityToPressure(d1) + DensityToPressure(d2)) / 2.0f;
}

int randSign() {
	return (float)pow(-1, rand() % 2);
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

void MakeParticleGrid(Particle particles[]) {
	int n = (SEGMENTS * 3) + 3;
	int j = 0;
	int i = 0;
	int counter = 0;

	while (counter < NUM_PARTICLES) {
		int max = (WIDTH - RADIUS) / ((RADIUS * 2));
		if (i % max == 0) {
			j++;
			i = 0;
		}
		Particle c;
		c.size = n;
		c.curr.x = RADIUS * 2 + i * RADIUS * 2;
		c.curr.y = RADIUS + j * RADIUS * 2.5;
		c.prev.y = c.curr.y; //static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10))) * pow(-1, rand() % 2);;
		c.prev.x = c.curr.x +static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1))) * pow(-1, rand() % 2);
		c.acc.x = 0;
		c.acc.y = 0;
		particles[counter] = c;
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
	

	buffers[0] = VAO;
	buffers[1] = VBO;
	buffers[2] = EBO;
	return buffers;
}

void MakeExtraParticles(Particle particles[]) {
	int i = NUM_PARTICLES;
	int n = (SEGMENTS * 3) + 3;
	
	while(i < MAX_PARTICLES) {
		Particle c;
		c.size = n;
		c.curr.x = 0;
		c.curr.y = 0;
		c.prev.y = 0;
		c.prev.x = 0;
		c.acc.x = 0;
		c.acc.y = 0;
		particles[i] = c;
		i++;

	}
}

void AddParticle(Particle particles[], double xpos, double ypos) {
	int n = (SEGMENTS * 3) + 3;

	if (NUM_PARTICLES < MAX_PARTICLES) {
		particles[NUM_PARTICLES].size = n;
		particles[NUM_PARTICLES].curr.x = (float)xpos;
		particles[NUM_PARTICLES].curr.y = (float)ypos;
		particles[NUM_PARTICLES].prev.y = (float)ypos;
		particles[NUM_PARTICLES].prev.x = (float)xpos + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1))) * pow(-1, rand() % 2);
		particles[NUM_PARTICLES].acc.x = 0;
		particles[NUM_PARTICLES].acc.y = 0;
		NUM_PARTICLES++;
	}
}