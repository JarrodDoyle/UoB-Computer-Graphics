#include "Model.h"

Model::Model() = default;
Model::Model(std::string n, const std::vector<ModelTriangle> &f, const std::vector<Colour> &m) :
        name(std::move(n)), faces(f), materials(m) {}