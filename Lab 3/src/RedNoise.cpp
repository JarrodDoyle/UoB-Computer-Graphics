#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <CanvasTriangle.h>
#include <Colour.h>

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

void drawLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour) {
	float xDiff = to.x - from.x;
	float yDiff = to.y - from.y;
	float numberOfSteps = abs(xDiff) > abs(yDiff) ? abs(xDiff) : abs(yDiff);
	float xStep = xDiff / numberOfSteps;
	float yStep = yDiff / numberOfSteps;
	float x = from.x;
	float y = from.y;
	for (int i=0; i<numberOfSteps; i++) {
		x += xStep;
		y += yStep;
		window.setPixelColour(round(x), round(y), (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue);
	}
}

void drawStrokedTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour colour) {
	drawLine(window, triangle[0], triangle[1], colour);
	drawLine(window, triangle[0], triangle[2], colour);
	drawLine(window, triangle[1], triangle[2], colour);
}

void drawRandomStrokedTriangle(DrawingWindow &window) {
	CanvasTriangle triangle = CanvasTriangle(
		CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1)),
		CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1)),
		CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1))
	);

	drawStrokedTriangle(window, triangle, Colour(rand()%256, rand()%256, rand()%256));
}

void draw(DrawingWindow &window) {
	window.clearPixels();
	for (int i=0; i<10; i++) {
		drawLine(
			window,
			CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1)),
			CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1)),
			Colour(rand()%256, rand()%256, rand()%256)
		);
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
		else if (event.key.keysym.sym == SDLK_u) drawRandomStrokedTriangle(window);
		else if (event.key.keysym.sym == SDLK_c) window.clearPixels();
	} else if (event.type == SDL_MOUSEBUTTONDOWN) window.savePPM("output.ppm");
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) {
			handleEvent(event, window);
			// update(window);
			// draw(window);
		}
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
