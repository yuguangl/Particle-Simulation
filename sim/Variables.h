#pragma once
#ifndef VARIABLES_H
#define VARIABLES_H

//#include<iostream>
#include <unordered_map>
#include <vector>

struct Tuple {
	int pId;
	int hId;

	bool operator< (const Tuple& o) const{
		return hId < o.hId;
	}
};



using namespace std;

#define PI 3.1415926535897932384626433
#define SEGMENTS 25 // just dont go over 90 for some reason
#define RADIUS 15 // <= 1
#define WIDTH 1000
#define HEIGHT 1000
#define TARGET_FPS 60
#define G 300
#define TIME_STEP (1.0f / 70.0f)
#define NUM_SUBSTEPS 1
#define SUBSTEP TIME_STEP/NUM_SUBSTEPS
#define SMOOTHING_RADIUS RADIUS*8
#define CELL_SIZE 50
#define NUM_CELLS_X ceil(WIDTH/CELL_SIZE)
#define NUM_CELLS_Y ceil(HEIGHT/CELL_SIZE)
#define MASS 1
#define PRESSUREC 100.0f
#define MAX_PARTICLES 1000

//globals
int NUM_PARTICLES = 300;
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
int a = 0;
//vector<Particle> particles;
//unordered_map<glm::vec2, vector<int>> spaceMap;
vector<float> property;
vector<float> densities;
vector<float> nearDensities;
float targetDensity = 1;
Tuple cellLookup[MAX_PARTICLES];
int groupIndices[MAX_PARTICLES];


#endif
