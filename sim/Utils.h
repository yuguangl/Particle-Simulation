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

float GetDistance(glm::vec2 p1, glm::vec2 p2) {
	vec2 diff = (p1 - p2);
	/*if (diff.x == 0 && (intiff.y == 0) {
		return 0.0f;
	}*/
	float dist = sqrt(diff.x * diff.x + diff.y * diff.y);
	//cout << dist << endl;
	return dist;
}
