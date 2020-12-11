#include "Model.h"

Model::Model() = default;
Model::Model(std::string n, const std::vector<ModelTriangle> &f, const glm::vec3 &position) :
        name(std::move(n)), faces(f), position(position) {}

std::ostream &operator<<(std::ostream &os, const Model &model) {
        os << model.name << "\n";
        for (int i=0; i<model.faces.size(); i++) {
                os << model.faces[i] << "\n";
        }
	return os;
}

void translateModel(std::vector<Model> &models, int modelIndex, glm::vec3 offset) {
        models[modelIndex].position += offset;
}