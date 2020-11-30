#include "Model.h"

Model::Model() = default;
Model::Model(std::string n, const std::vector<ModelTriangle> &f, const std::vector<Material> &m) :
        name(std::move(n)), faces(f), materials(m) {}

std::ostream &operator<<(std::ostream &os, const Model &model) {
        os << model.name << "\n";
        for (int i=0; i<model.faces.size(); i++) {
                os << model.faces[i] << "\n";
        }
        for (int i=0; i<model.materials.size(); i++) {
                os << model.materials[i] << "\n";
        }
	return os;
}