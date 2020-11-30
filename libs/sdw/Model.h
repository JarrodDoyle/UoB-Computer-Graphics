#pragma once

#include <iostream>
#include <vector>
#include "ModelTriangle.h"
#include "Colour.h"

struct Model {
    std::string name;
    std::vector<ModelTriangle> faces{};
    std::vector<Colour> materials{};
    Model();
    Model(std::string n, const std::vector<ModelTriangle> &f, const std::vector<Colour> &m);

};