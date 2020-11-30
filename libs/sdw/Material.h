#pragma once

#include "Colour.h"
#include "TextureMap.h"

struct Material {
    std::string name;
    Colour colour{};
    TextureMap textureMap{};
    Material();
    Material(std::string n, const Colour &c, const TextureMap &t);
    friend std::ostream &operator<<(std::ostream &os, const Material &material);
};