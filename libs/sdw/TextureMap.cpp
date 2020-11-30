#include "TextureMap.h"

TextureMap::TextureMap() = default;
TextureMap::TextureMap(size_t w, size_t h, const std::vector<uint32_t> &p) :
		width(w), height(h), pixels(p) {};

TextureMap loadTextureMap(const std::string &filename) {
	std::ifstream inputStream(filename, std::ifstream::in);
	std::string nextLine;
	// Get the "P6" magic number
	std::getline(inputStream, nextLine);
	// Read the width and height line
	std::getline(inputStream, nextLine);
	// Skip over any comment lines !
	while (nextLine.at(0) == '#') std::getline(inputStream, nextLine);
	auto widthAndHeight = split(nextLine, ' ');
	if (widthAndHeight.size() != 2)
		throw std::invalid_argument("Failed to parse width and height line, line was `" + nextLine + "`");

	auto width = std::stoi(widthAndHeight[0]);
	auto height = std::stoi(widthAndHeight[1]);
	// std::cout << "[loadTextureMap: " << width << "," << height << "]" << std::endl;
	// Read the max value (which we assume is 255)
	std::getline(inputStream, nextLine);

	std::vector<uint32_t> pixels;
	pixels.resize(width * height);
	for (size_t i = 0; i < width * height; i++) {
		int red = inputStream.get();
		int green = inputStream.get();
		int blue = inputStream.get();
		pixels[i] = ((255 << 24) + (red << 16) + (green << 8) + (blue));
	}
	inputStream.close();
	return TextureMap(width, height, pixels);
}

std::ostream &operator<<(std::ostream &os, const TextureMap &map) {
	os << "(" << map.width << " x " << map.height << ")";
	return os;
}
