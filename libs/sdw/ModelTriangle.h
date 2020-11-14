#pragma once

#include <glm/glm.hpp>
#include <string>
#include <array>
#include "Colour.h"
#include "TexturePoint.h"

struct ModelTriangle {
	std::array<glm::vec4, 3> vertices{};
	std::array<TexturePoint, 3> texturePoints{};
	Colour colour{};
	glm::vec3 normal{};

	ModelTriangle();
	ModelTriangle(const glm::vec4 &v0, const glm::vec4 &v1, const glm::vec4 &v2, Colour trigColour);
	friend std::ostream &operator<<(std::ostream &os, const ModelTriangle &triangle);
};
