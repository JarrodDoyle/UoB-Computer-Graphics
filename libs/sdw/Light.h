#pragma once

#include <glm/glm.hpp>
#include "Colour.h"

struct Light {
    glm::vec3 position{};
    uint32_t size{};
    float strength{};
    Colour colour{};
    Light();
    Light(const glm::vec3 &pos, uint32_t size, float strength, const Colour &colour);
};