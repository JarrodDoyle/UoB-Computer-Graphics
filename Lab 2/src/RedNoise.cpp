#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>

#define WIDTH 320
#define HEIGHT 240

std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues) {
	std::vector<float> interpolatedValues;
	float stepValue = (to - from) / (numberOfValues - 1);
	for (int i=0; i<numberOfValues; i++) {
		interpolatedValues.push_back(from + (i * stepValue));
	}
	return interpolatedValues;
}


std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues) {
	std::vector<glm::vec3> interpolatedValues;
	auto xs = interpolateSingleFloats(from[0], to[0], numberOfValues);
	auto ys = interpolateSingleFloats(from[1], to[1], numberOfValues);
	auto zs = interpolateSingleFloats(from[2], to[2], numberOfValues);
	for (int i=0; i<numberOfValues; i++) {
		glm::vec3 v(xs[i], ys[i], zs[i]);
		interpolatedValues.push_back(v);
	}
	return interpolatedValues;
}

void draw(DrawingWindow &window) {
	window.clearPixels();
	glm::vec3 red(255, 0, 0);
	glm::vec3 blue(0, 0, 255);
	glm::vec3 green(0, 255, 0);
	glm::vec3 yellow(255, 255, 0);
	auto firstColumn = interpolateThreeElementValues(red, yellow, window.height);
	auto lastColumn = interpolateThreeElementValues(blue, green, window.height);
	for (size_t y = 0; y < window.height; y++) {
		auto horizontalValues = interpolateThreeElementValues(firstColumn[y], lastColumn[y], window.width);
		for (size_t x = 0; x < window.width; x++) {
			glm::vec3 value = horizontalValues[x];
			uint32_t colour = (255 << 24) + ((int)value[0] << 16) + ((int)value[1] << 8) + (int)value[2];
			window.setPixelColour(x, y, colour);
		}
	}
}

void update(DrawingWindow &window) {
	// Function for performing animation (shifting artifacts or moving the camera)
}

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) window.savePPM("output.ppm");
}

int main(int argc, char *argv[]) {
	std::vector<glm::vec3> result;
	glm::vec3 from(1, 4, 9.2);
	glm::vec3 to(4, 1, 9.8);
	result = interpolateThreeElementValues(from, to, 4);
	for(size_t i=0; i<result.size(); i++) std::cout << "(" << result[i][0] << " " << result[i][1] << " " << result[i][2] << ")" ;
	std::cout << std::endl;

	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		update(window);
		draw(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
