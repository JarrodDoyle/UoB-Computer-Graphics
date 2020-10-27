#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <CanvasTriangle.h>
#include <Colour.h>
#include <ModelTriangle.h>
#include <TextureMap.h>
#include <TexturePoint.h>
#include <Utils.h>

#define WIDTH 320
#define HEIGHT 240

std::vector<ModelTriangle> loadObjFile(const std::string &filename, float scale) {
	std::vector<glm::vec3> vertices;
	std::vector<ModelTriangle> faces;

	std::ifstream inputStream(filename, std::ifstream::in);
	std::string nextLine;
	while (std::getline(inputStream, nextLine)) {
		auto vector = split(nextLine, ' ');
		if (vector[0] == "v") {
			vertices.push_back(glm::vec3(
				std::stof(vector[1]) * scale,
				std::stof(vector[2]) * scale,
				std::stof(vector[3]) * scale
			));
		}
		else if (vector[0] == "f") {
			faces.push_back(ModelTriangle(
				vertices[std::stoi(split(vector[1], '/')[0]) - 1],
				vertices[std::stoi(split(vector[2], '/')[0]) - 1],
				vertices[std::stoi(split(vector[3], '/')[0]) - 1],
				Colour(255, 255, 255)
			));
		}
	}
	return faces;
}

std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues) {
	std::vector<float> interpolatedValues;
	float stepValue = (to - from) / (numberOfValues - 1);
	for (int i=0; i<numberOfValues; i++) {
		interpolatedValues.push_back(from + (i * stepValue));
	}
	return interpolatedValues;
}

template <typename T>
std::vector<CanvasPoint> interpolatePointsRounded(T from, T to, int numberOfValues) {
	std::vector<CanvasPoint> interpolatedValues;
	auto xs = interpolateSingleFloats(from.x, to.x, numberOfValues);
	auto ys = interpolateSingleFloats(from.y, to.y, numberOfValues);
	for (int i=0; i<numberOfValues; i++) {
		interpolatedValues.push_back(CanvasPoint(round(xs[i]), round(ys[i])));
	}
	return interpolatedValues;
}

float getNumberOfSteps(CanvasPoint from, CanvasPoint to) {
	return fmax(fmax(abs(to.x - from.x), abs(to.y - from.y)), 1);
}

void drawLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to, float numberOfSteps, std::vector<Colour> colours) {
	auto points = interpolatePointsRounded(from, to, numberOfSteps + 1);
	for (float i=0.0; i<=numberOfSteps; i++) {
		uint32_t c = (255 << 24) + (colours[i].red << 16) + (colours[i].green << 8) + colours[i].blue;
		window.setPixelColour(points[i].x, points[i].y, c);
	}
}

void drawLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour) {
	float numberOfSteps = getNumberOfSteps(from, to);
	drawLine(window, from, to, numberOfSteps, std::vector<Colour>(numberOfSteps, colour));
}

template <typename T>
std::vector<CanvasPoint> getSidedPoints(T p1, T p2, T p3, float height1, float height2) {
	auto points1 = interpolatePointsRounded(p1, p2, height1);
	auto points2 = interpolatePointsRounded(p2, p3, height2);
	points1.pop_back();
	points1.insert(points1.end(), points2.begin(), points2.end());
	return points1;
}

std::vector<CanvasPoint> getSidedPoints(CanvasTriangle triangle, float height1, float height2) {
	return getSidedPoints(triangle[0], triangle[1], triangle[2], height1, height2);
}

CanvasTriangle generateTriangle(DrawingWindow &window) {
	return CanvasTriangle(
		CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1)),
		CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1)),
		CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1))
	);
}

CanvasTriangle generateTexturedTriangle() {
	CanvasTriangle triangle = CanvasTriangle(CanvasPoint(160, 10), CanvasPoint(300, 230), CanvasPoint(10, 150));
	triangle[0].texturePoint = TexturePoint(195, 5);
	triangle[1].texturePoint = TexturePoint(395, 380);
	triangle[2].texturePoint = TexturePoint(65, 330);
	return triangle;
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
	auto starts = getSidedPoints(triangle, triangle[1].y - triangle[0].y + 1, triangle[2].y - triangle[1].y + 1);
	auto ends = interpolatePointsRounded(triangle[0], triangle[2], triangle[2].y - triangle[0].y + 1);

	// Draw the filled in triangle then do an outline
	for (int i=0; i<=triangle[2].y - triangle[0].y; i++) {
		drawLine(window, starts[i], ends[i], colour);
	}

	drawStrokedTriangle(window, triangle, Colour(255, 255, 255));
}

void drawTexturedTriangle(DrawingWindow &window, CanvasTriangle triangle, TextureMap &texMap) {
	sortTriangleVertices(triangle);

	float height1 = triangle[1].y - triangle[0].y + 1;
	float height2 = triangle[2].y - triangle[1].y + 1;

	auto canvasStarts = getSidedPoints(triangle, height1, height2);
	auto canvasEnds = interpolatePointsRounded(triangle[0], triangle[2], height1 + height2 - 1);

	auto textureStarts = getSidedPoints(triangle[0].texturePoint, triangle[1].texturePoint, triangle[2].texturePoint, height1, height2);
	auto textureEnds = interpolatePointsRounded(triangle[0].texturePoint, triangle[2].texturePoint, height1 + height2 - 1);

	for(int i=0; i<=height1 + height2 - 2; i++) {
		float numberOfSteps = getNumberOfSteps(canvasStarts[i], canvasEnds[i]);
		auto points = interpolatePointsRounded(textureStarts[i], textureEnds[i], numberOfSteps + 1);

		std::vector<Colour> colours;
		for (float i=0.0; i<=numberOfSteps; i++) {
			uint32_t c = texMap.pixels[(round(points[i].y) * texMap.width) + round(points[i].x)];
			colours.push_back(Colour((c & 0xFF0000) >> 16, (c & 0xFF00) >> 8, (c & 0xFF)));
		}

		drawLine(window, canvasStarts[i], canvasEnds[i], numberOfSteps, colours);
	}

	drawStrokedTriangle(window, triangle, Colour(255, 255, 255));
}

void update(DrawingWindow &window) {
	// Function for performing animation (shifting artifacts or moving the camera)
}

void handleEvent(SDL_Event event, DrawingWindow &window, TextureMap &texMap) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
		else if (event.key.keysym.sym == SDLK_c) window.clearPixels();
		else if (event.key.keysym.sym == SDLK_u) drawStrokedTriangle(window, generateTriangle(window), Colour(rand()%256, rand()%256, rand()%256));
		else if (event.key.keysym.sym == SDLK_f) drawFilledTriangle(window, generateTriangle(window), Colour(rand()%256, rand()%256, rand()%256));
		else if (event.key.keysym.sym == SDLK_t) drawTexturedTriangle(window, generateTexturedTriangle(), texMap);
	} else if (event.type == SDL_MOUSEBUTTONDOWN) window.savePPM("output.ppm");
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	TextureMap texMap = TextureMap("texture.ppm");
	
	float vertexScale = 0.17;

	std::vector<ModelTriangle> faces = loadObjFile("models/cornell-box.obj", vertexScale);
	for (int i=0; i<faces.size(); i++) {
		std::cout << faces[i] << std::endl;
	}

	SDL_Event event;
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) {
			handleEvent(event, window, texMap);
			// update(window);
		}
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
