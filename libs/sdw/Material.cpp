#include "Material.h"

Material::Material() = default;
Material::Material(std::string n, const Colour &c, const TextureMap &t) :
    name(std::move(n)), colour(c), textureMap(t) {}

std::ostream &operator<<(std::ostream &os, const Material &material) {
        os << material.name << "\n";
        os << material.colour << "\n";
        os << material.textureMap << "\n";
	return os;
}