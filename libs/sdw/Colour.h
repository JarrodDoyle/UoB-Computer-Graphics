#pragma once

#include <iostream>
#include "TextureMap.h"

struct Colour {
	std::string name;
	int red{};
	int green{};
	int blue{};
	Colour();
	Colour(int r, int g, int b);
	Colour(std::string n, int r, int g, int b);
};

std::ostream &operator<<(std::ostream &os, const Colour &colour);
