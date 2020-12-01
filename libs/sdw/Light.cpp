#include "Light.h"

Light::Light() = default;
Light::Light(const glm::vec3 &pos, uint32_t size, float strength, const Colour &colour) :
    position(pos), size(size), strength(strength), colour(colour) {}