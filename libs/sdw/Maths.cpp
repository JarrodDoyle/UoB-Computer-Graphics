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