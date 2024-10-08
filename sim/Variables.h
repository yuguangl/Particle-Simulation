#pragma once
#ifndef VARIABLES_H
#define VARIABLES_H

//#include<iostream>
#include <unordered_map>
#include <vector>

struct Tuple {
	//wtf is this
	int pId;
	int hId;

	bool operator< (const Tuple& o)const {
		return hId < o.hId;
	}
};



using namespace std;

#define PI 3.1415926535897932384626433
#define SEGMENTS 10 // just dont go over 90 for some reason
#define RADIUS 20 // >= 4 for some reason AND NOT 7 BECAUSE WEIRD BUG HAPPENS maybe hashing issue again?
#define WIDTH 1200
#define HEIGHT 1200
#define TARGET_FPS 100
#define G 300
#define TIME_STEP (1.0f / TARGET_FPS)
#define NUM_SUBSTEPS 2 //bruh htis is helpful but my frames go to like 0
#define SUBSTEP (TIME_STEP/NUM_SUBSTEPS)
#define SMOOTHING_RADIUS (RADIUS * 6)
#define CELL_SIZE (RADIUS * 6)
#define NUM_CELLS_X (int)ceil(WIDTH/CELL_SIZE)
#define NUM_CELLS_Y (int)ceil(HEIGHT/CELL_SIZE)
#define MASS 1
#define PRESSUREC 1000.0f
#define MAX_PARTICLES 20000
#define NUM_THREADS 5

//globals
int NUM_PARTICLES = 10; //if theres no particles it breaks
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
float densities[MAX_PARTICLES];
float nearDensities[MAX_PARTICLES];
float targetDensity = 1;
Tuple cellLookup[MAX_PARTICLES];
int groupIndices[MAX_PARTICLES];
int hashes[MAX_PARTICLES];
bool mouseForce = false;
GLFWwindow* window;



#endif
