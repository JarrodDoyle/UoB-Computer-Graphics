#pragma once

#include <iostream>
#include <fstream>
#include <stdexcept>
#include "Utils.h"

struct TextureMap {
	size_t width;
	size_t height;
	std::vector<uint32_t> pixels;
	TextureMap();
	TextureMap(size_t w, size_t h, const std::vector<uint32_t> &p);
	friend std::ostream &operator<<(std::ostream &os, const TextureMap &point);
};

TextureMap loadTextureMap(const std::string &filename);