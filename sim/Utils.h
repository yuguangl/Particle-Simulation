#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include "Variables.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
using namespace glm;

float GetDistance(vec2 p1, vec2 p2) {
	vec2 diff = (p1 - p2);
	return sqrt(diff.x * diff.x + diff.y * diff.y);
}




