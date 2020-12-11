#pragma once

#include <iostream>
#include <vector>
#include "ModelTriangle.h"
#include "Material.h"

struct Model {
    std::string name;
    std::vector<ModelTriangle> faces{};
    glm::vec3 position{};
    Model();
    Model(std::string n, const std::vector<ModelTriangle> &f, const glm::vec3 &position);
    friend std::ostream &operator<<(std::ostream &os, const Model &model);
};

void translateModel(std::vector<Model> &models, int modelIndex, glm::vec3 offset);