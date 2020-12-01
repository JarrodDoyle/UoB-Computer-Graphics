#include "Model.h"

Model::Model() = default;
Model::Model(std::string n, const std::vector<ModelTriangle> &f) :
        name(std::move(n)), faces(f){}

std::ostream &operator<<(std::ostream &os, const Model &model) {
        os << model.name << "\n";
        for (int i=0; i<model.faces.size(); i++) {
                os << model.faces[i] << "\n";
        }
	return os;
}