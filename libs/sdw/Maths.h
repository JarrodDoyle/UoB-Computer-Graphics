#pragma once

#include <glm/glm.hpp>

glm::mat4 translationMatrix(glm::vec3 vector);

glm::mat4 rotationMatrixX(float radians);
glm::mat4 rotationMatrixY(float radians);
glm::mat4 rotationMatrixZ(float radians);