#pragma once

#include <glm/glm.hpp>
#include <string>
#include <array>
#include "Material.h"
#include "TexturePoint.h"

struct ModelTriangle {
	std::array<glm::vec4, 3> vertices{};
	std::array<TexturePoint, 3> texturePoints{};
	std::array<glm::vec3, 3> vertexNormals{};
	Material material{};
	glm::vec3 normal{};

	ModelTriangle();
	ModelTriangle(const glm::vec4 &v0, const glm::vec4 &v1, const glm::vec4 &v2, const Material &m);
	friend std::ostream &operator<<(std::ostream &os, const ModelTriangle &triangle);
};
