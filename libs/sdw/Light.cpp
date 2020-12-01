#include "Light.h"

Light::Light() = default;
Light::Light(const glm::vec3 &pos, uint32_t size, float strength) :
    position(pos), size(size), strength(strength) {}