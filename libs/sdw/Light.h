#pragma once

#include <glm/glm.hpp>

struct Light {
    glm::vec3 position{};
    uint32_t size{};
    float strength{};
    Light();
    Light(const glm::vec3 &pos, uint32_t size, float strength);
};