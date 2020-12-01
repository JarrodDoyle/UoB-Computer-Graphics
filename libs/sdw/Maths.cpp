#include "Maths.h"

glm::mat4 translationMatrix(glm::vec3 v) {
    return {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        v[0], v[1], v[2], 1.0f
    };
}

glm::mat4 rotationMatrixX(float rads) {
    return {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cos(rads), sin(rads), 0.0f,
        0.0f, -sin(rads),  cos(rads), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

glm::mat4 rotationMatrixY(float rads) {
    return {
        cos(rads), 0.0f, -sin(rads), 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sin(rads), 0.0f, cos(rads), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

glm::mat4 rotationMatrixZ(float rads) {
    return {
        cos(rads), sin(rads), 0.0f, 0.0f,
        -sin(rads), cos(rads), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

glm::mat4 lookAt(glm::mat4 &camera, glm::vec3 target) {
	glm::vec3 z = glm::normalize(glm::vec3(camera[3] / camera[3][3]) - target);
	glm::vec3 x = glm::normalize(glm::cross(glm::vec3(0.0, 1.0, 0.0), z));
	glm::vec3 y = glm::normalize(glm::cross(z, x));
	return glm::mat4(
		glm::vec4(x, 0.0),
		glm::vec4(y, 0.0),
		glm::vec4(z, 0.0),
		glm::vec4(camera[3])
	);
}