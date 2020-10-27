#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <CanvasTriangle.h>
#include <Colour.h>
#include <TextureMap.h>
#include <TexturePoint.h>

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

std::vector<float> getTriangleRowStarts(CanvasTriangle triangle) {
	auto xStarts = interpolateSingleFloats(triangle[0].x, triangle[1].x, triangle[1].y - triangle[0].y + 1);
	xStarts.pop_back(); // Last row is duplicated by next "triangle" so just drop this first version of it
	auto xs = interpolateSingleFloats(triangle[1].x, triangle[2].x, triangle[2].y - triangle[1].y + 1);
	xStarts.insert(xStarts.end(), xs.begin(), xs.end());
	return xStarts;
}

void drawLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour) {
	uint32_t c = (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;
	
	float xDiff = to.x - from.x;
	float yDiff = to.y - from.y;
	float numberOfSteps = abs(xDiff) > abs(yDiff) ? abs(xDiff) : abs(yDiff);
	if (numberOfSteps == 0.0) numberOfSteps = 1;

	auto xs = interpolateSingleFloats(from.x, to.x, numberOfSteps + 1);
	auto ys = interpolateSingleFloats(from.y, to.y, numberOfSteps + 1);
	for (float i=0.0; i<=numberOfSteps; i++) {
		window.setPixelColour(round(xs[i]), round(ys[i]), c);
	}
}

void drawLineTextured(DrawingWindow &window, TextureMap &texMap, CanvasPoint fromCanvas, CanvasPoint toCanvas, CanvasPoint fromTex, CanvasPoint toTex) {
	float xDiff = toCanvas.x - fromCanvas.x;
	float yDiff = toCanvas.y - fromCanvas.y;
	float numberOfSteps = abs(xDiff) > abs(yDiff) ? abs(xDiff) : abs(yDiff);
	if (numberOfSteps == 0.0) numberOfSteps = 1;

	auto xsCanvas = interpolateSingleFloats(fromCanvas.x, toCanvas.x, numberOfSteps + 1);
	auto ysCanvas = interpolateSingleFloats(fromCanvas.y, toCanvas.y, numberOfSteps + 1);
	auto xsTex = interpolateSingleFloats(fromTex.x, toTex.x, numberOfSteps + 1);
	auto ysTex = interpolateSingleFloats(fromTex.y, toTex.y, numberOfSteps + 1);

	for (float i=0.0; i<=numberOfSteps; i++) {
		window.setPixelColour(round(xsCanvas[i]), round(ysCanvas[i]), texMap.pixels[(round(ysTex[i]) * texMap.width) + round(xsTex[i])]);
	}
}

CanvasTriangle generateTriangle(DrawingWindow &window) {
	return CanvasTriangle(
		CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1)),
		CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1)),
		CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1))
	);
}

void sortTriangleVertices(CanvasTriangle &triangle) {
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
}

void drawStrokedTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour colour) {
	drawLine(window, triangle[0], triangle[1], colour);
	drawLine(window, triangle[0], triangle[2], colour);
	drawLine(window, triangle[1], triangle[2], colour);
}

void drawFilledTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour colour) {
	sortTriangleVertices(triangle);

	// Work out the start and end xPos of each row of the triangle
	auto xStarts = getTriangleRowStarts(triangle);
	auto xEnds = interpolateSingleFloats(triangle[0].x, triangle[2].x, triangle[2].y - triangle[0].y + 1);

	// Draw the filled in triangle then do an outline
	float y = triangle[0].y;
	for (int i=0; i<=triangle[2].y - triangle[0].y; i++) {
		drawLine(window, CanvasPoint(round(xStarts[i]), y), CanvasPoint(round(xEnds[i]), y), colour);
		y++;
	}

	drawStrokedTriangle(window, triangle, Colour(255, 255, 255));
}

void drawTexturedTriangle(DrawingWindow &window, TextureMap &texMap) {
	CanvasTriangle cTriangle = CanvasTriangle(CanvasPoint(160, 10), CanvasPoint(300, 230), CanvasPoint(10, 150));
	cTriangle[0].texturePoint = TexturePoint(195, 5);
	cTriangle[1].texturePoint = TexturePoint(395, 380);
	cTriangle[2].texturePoint = TexturePoint(65, 330);
	
	sortTriangleVertices(cTriangle);

	auto xStartsCanvas = getTriangleRowStarts(cTriangle);
	auto xEndsCanvas = interpolateSingleFloats(cTriangle[0].x, cTriangle[2].x, cTriangle[2].y - cTriangle[0].y + 1);

	auto xStartsTex = interpolateSingleFloats(cTriangle[0].texturePoint.x, cTriangle[1].texturePoint.x, cTriangle[1].y - cTriangle[0].y + 1);
	xStartsTex.pop_back(); // Last row is duplicated by next "triangle" so just drop this first version of it
	auto xs = interpolateSingleFloats(cTriangle[1].texturePoint.x, cTriangle[2].texturePoint.x, cTriangle[2].y - cTriangle[1].y + 1);
	xStartsTex.insert(xStartsTex.end(), xs.begin(), xs.end());
	auto xEndsTex = interpolateSingleFloats(cTriangle[0].texturePoint.x, cTriangle[2].texturePoint.x, cTriangle[2].y - cTriangle[0].y + 1);

	auto yStartsTex = interpolateSingleFloats(cTriangle[0].texturePoint.y, cTriangle[1].texturePoint.y, cTriangle[1].y - cTriangle[0].y + 1);
	yStartsTex.pop_back(); // Last row is duplicated by next "triangle" so just drop this first version of it
	auto ys = interpolateSingleFloats(cTriangle[1].texturePoint.y, cTriangle[2].texturePoint.y, cTriangle[2].y - cTriangle[1].y + 1);
	yStartsTex.insert(yStartsTex.end(), ys.begin(), ys.end());
	auto yEndsTex = interpolateSingleFloats(cTriangle[0].texturePoint.y, cTriangle[2].texturePoint.y, cTriangle[2].y - cTriangle[0].y + 1);

	// Draw the filled in triangle then do an outline
	float y = cTriangle[0].y;
	for (int i=0; i<=cTriangle[2].y - cTriangle[0].y; i++) {
		drawLineTextured(
			window,
			texMap,
			CanvasPoint(round(xStartsCanvas[i]), y),
			CanvasPoint(round(xEndsCanvas[i]), y),
			CanvasPoint(round(xStartsTex[i]), round(yStartsTex[i])),
			CanvasPoint(round(xEndsTex[i]), round(yEndsTex[i]))
		);
		// drawLine(window, CanvasPoint(round(xStartsCanvas[i]), y), CanvasPoint(round(xEndsCanvas[i]), y), colour);
		y++;
	}

	drawStrokedTriangle(window, cTriangle, Colour(255, 255, 255));
}

// void update(DrawingWindow &window) {
// 	// Function for performing animation (shifting artifacts or moving the camera)
// }

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
		else if (event.key.keysym.sym == SDLK_u) drawStrokedTriangle(window, generateTriangle(window), Colour(rand()%256, rand()%256, rand()%256));
		else if (event.key.keysym.sym == SDLK_f) drawFilledTriangle(window, generateTriangle(window), Colour(rand()%256, rand()%256, rand()%256));
	} else if (event.type == SDL_MOUSEBUTTONDOWN) window.savePPM("output.ppm");
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	TextureMap texMap = TextureMap("texture.ppm");
	SDL_Event event;
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) {
			// window.clearPixels();
			handleEvent(event, window);
			// update(window);
			drawTexturedTriangle(window, texMap);
		}
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
