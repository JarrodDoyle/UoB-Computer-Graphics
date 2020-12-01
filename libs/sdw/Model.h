#pragma once

#include <iostream>
#include <vector>
#include "ModelTriangle.h"
#include "Material.h"

struct Model {
    std::string name;
    std::vector<ModelTriangle> faces{};
    Model();
    Model(std::string n, const std::vector<ModelTriangle> &f);
    friend std::ostream &operator<<(std::ostream &os, const Model &model);
};