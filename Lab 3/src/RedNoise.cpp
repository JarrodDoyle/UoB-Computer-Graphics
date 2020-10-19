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
	if (numberOfSteps == 0.0) numberOfSteps = 1;
	float xStep = xDiff / numberOfSteps;
	float yStep = yDiff / numberOfSteps;
	for (float i=0.0; i<=numberOfSteps; i++) {
		uint32_t c = (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;
		window.setPixelColour(round(from.x + (i * xStep)), round(from.y + (i * yStep)), c);
	}
}


CanvasTriangle generateTriangle(DrawingWindow &window) {
	return CanvasTriangle(
		CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1)),
		CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1)),
		CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1))
	);
}

void drawStrokedTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour colour) {
	drawLine(window, triangle[0], triangle[1], colour);
	drawLine(window, triangle[0], triangle[2], colour);
	drawLine(window, triangle[1], triangle[2], colour);
}

void drawRandomStrokedTriangle(DrawingWindow &window) {
	drawStrokedTriangle(window, generateTriangle(window), Colour(rand()%256, rand()%256, rand()%256));
}

void drawFilledTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour colour) {
	// Sort triangle point positions
	if (triangle[0].y > triangle[1].y) {
		std::swap(triangle[0], triangle[1]);
	}
	if (triangle[1].y > triangle[2].y) {
		std::swap(triangle[1], triangle[2]);
		if (triangle[0].y > triangle[1].y) {
			std::swap(triangle[0], triangle[1]);
		}	
	}

	// Work out the start and end xPos of each row of the triangle
	auto xStarts = interpolateSingleFloats(triangle[0].x, triangle[1].x, triangle[1].y - triangle[0].y + 1);
	xStarts.pop_back(); // Last row is duplicated by next "triangle" so just drop this first version of it
	auto xs = interpolateSingleFloats(triangle[1].x, triangle[2].x, triangle[2].y - triangle[1].y + 1);
	xStarts.insert(xStarts.end(), xs.begin(), xs.end());
	auto xEnds = interpolateSingleFloats(triangle[0].x, triangle[2].x, triangle[2].y - triangle[0].y + 1);

	// Draw the filled in triangle then do an outline
	float y = triangle[0].y;
	for (int i=0; i<=triangle[2].y - triangle[0].y; i++) {
		drawLine(window, CanvasPoint(round(xStarts[i]), y), CanvasPoint(round(xEnds[i]), y), colour);
		y++;
	}

	drawStrokedTriangle(window, triangle, Colour(255, 255, 255));
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
		else if (event.key.keysym.sym == SDLK_f) drawFilledTriangle(window, generateTriangle(window), Colour(rand()%256, rand()%256, rand()%256));
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
