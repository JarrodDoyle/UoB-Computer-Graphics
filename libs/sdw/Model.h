#pragma once

#include <iostream>
#include <vector>
#include "ModelTriangle.h"
#include "Material.h"

struct Model {
    std::string name;
    std::vector<ModelTriangle> faces{};
    std::vector<Material> materials{};
    Model();
    Model(std::string n, const std::vector<ModelTriangle> &f, const std::vector<Material> &m);
    friend std::ostream &operator<<(std::ostream &os, const Model &model);
};