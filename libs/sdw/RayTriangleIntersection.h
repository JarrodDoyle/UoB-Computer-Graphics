#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include "ModelTriangle.h"

struct RayTriangleIntersection {
	glm::vec3 intersectionPoint;
	float distanceFromCamera;
	ModelTriangle intersectedTriangle;
	size_t triangleIndex;
	size_t modelIndex;

	RayTriangleIntersection();
	RayTriangleIntersection(const glm::vec3 &point, float distance, const ModelTriangle &triangle, size_t index, size_t modIndex);
	friend std::ostream &operator<<(std::ostream &os, const RayTriangleIntersection &intersection);
};
