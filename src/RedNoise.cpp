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

#define WIDTH 640
#define HEIGHT 480

std::vector<Colour> loadMtlFile(const std::string &filename) {
	std::vector<Colour> colours;

	std::ifstream inputStream(filename, std::ifstream::in);
	std::string nextLine;
	std::string name;

	while (std::getline(inputStream, nextLine)) {
		auto line = split(nextLine, ' ');
		
		if (line[0] == "newmtl") {
			name = line[1];
		} else if (line[0] == "Kd") {
			colours.push_back(Colour(
				name,
				(int)(std::stof(line[1]) * 255),
				(int)(std::stof(line[2]) * 255),
				(int)(std::stof(line[3]) * 255)
			));
		}
	}
	return colours;
}

std::vector<ModelTriangle> loadObjFile(const std::string &filename, float scale) {
	std::vector<glm::vec3> vertices;
	std::vector<ModelTriangle> faces;

	std::ifstream inputStream(filename, std::ifstream::in);
	std::string nextLine;
	std::vector<Colour> materials;
	Colour colour = Colour(255, 255, 255);

	while (std::getline(inputStream, nextLine)) {
		auto vector = split(nextLine, ' ');
		if (vector[0] == "mtllib") {
			materials = loadMtlFile(vector[1]);
		}
		else if (vector[0] == "usemtl") {
			for (int i=0; i<materials.size(); i++) {
				if (materials[i].name == vector[1]) {
					colour = materials[i];
					break;
				}
			}
		}
		else if (vector[0] == "v") {
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
				colour
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

std::vector<CanvasPoint> interpolatePointsRounded(CanvasPoint from, CanvasPoint to, int numberOfValues) {
	std::vector<CanvasPoint> interpolatedValues;
	auto xs = interpolateSingleFloats(from.x, to.x, numberOfValues);
	auto ys = interpolateSingleFloats(from.y, to.y, numberOfValues);
	auto depths = interpolateSingleFloats(from.depth, to.depth, numberOfValues);
	for (int i=0; i<numberOfValues; i++) {
		interpolatedValues.push_back(CanvasPoint(round(xs[i]), round(ys[i]), depths[i]));
	}
	return interpolatedValues;
}

std::vector<CanvasPoint> interpolatePointsRounded(TexturePoint from, TexturePoint to, int numberOfValues) {
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

void drawLine(DrawingWindow &window, std::vector<float> &depthBuffer, CanvasPoint from, CanvasPoint to, float numberOfSteps, std::vector<Colour> colours) {
	auto points = interpolatePointsRounded(from, to, numberOfSteps + 1);
	for (float i=0.0; i<=numberOfSteps; i++) {
		int depthIndex = (points[i].y * window.width) + points[i].x;
		float pointDepth = 1 / -points[i].depth;
		if (pointDepth > depthBuffer[depthIndex]) {
			depthBuffer[depthIndex] = pointDepth;
			uint32_t c = (255 << 24) + (colours[i].red << 16) + (colours[i].green << 8) + colours[i].blue;
			window.setPixelColour(points[i].x, points[i].y, c);
		}
	}
}

void drawLine(DrawingWindow &window, std::vector<float> &depthBuffer, CanvasPoint from, CanvasPoint to, Colour colour) {
	float numberOfSteps = getNumberOfSteps(from, to);
	drawLine(window, depthBuffer, from, to, numberOfSteps, std::vector<Colour>(numberOfSteps + 1, colour));
}

template <typename T>
std::vector<CanvasPoint> getSidedPoints(T p1, T p2, T p3, float height1, float height2) {
	auto points1 = interpolatePointsRounded(p1, p2, height1);
	if (height2 > 1) {
		auto points2 = interpolatePointsRounded(p2, p3, height2);
		points1.pop_back();
		points1.insert(points1.end(), points2.begin(), points2.end());
	}
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

void drawStrokedTriangle(DrawingWindow &window, std::vector<float> &depthBuffer, CanvasTriangle triangle, Colour colour) {
	drawLine(window, depthBuffer, triangle[0], triangle[1], colour);
	drawLine(window, depthBuffer, triangle[0], triangle[2], colour);
	drawLine(window, depthBuffer, triangle[1], triangle[2], colour);
}

void drawFilledTriangle(DrawingWindow &window, std::vector<float> &depthBuffer, CanvasTriangle triangle, Colour colour, bool outline) {
	sortTriangleVertices(triangle);

	// Work out the start and end xPos of each row of the triangle
	auto starts = getSidedPoints(triangle, triangle[1].y - triangle[0].y + 1, triangle[2].y - triangle[1].y + 1);
	auto ends = interpolatePointsRounded(triangle[0], triangle[2], triangle[2].y - triangle[0].y + 1);

	// Draw the filled in triangle then do an outline
	for (int i=0; i<=triangle[2].y - triangle[0].y; i++) {
		drawLine(window, depthBuffer, starts[i], ends[i], colour);
	}

	if (outline) drawStrokedTriangle(window, depthBuffer, triangle, Colour(255, 255, 255));
}

void drawTexturedTriangle(DrawingWindow &window, std::vector<float> &depthBuffer, CanvasTriangle triangle, TextureMap &texMap) {
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

		drawLine(window, depthBuffer, canvasStarts[i], canvasEnds[i], numberOfSteps, colours);
	}

	drawStrokedTriangle(window, depthBuffer, triangle, Colour(255, 255, 255));
}

void draw(DrawingWindow &window, std::vector<float> &depthBuffer, glm::vec3 camera, float focalLength, std::vector<ModelTriangle> faces) {
	float planeMultiplier = 100.0;
	for (int i=0; i<faces.size(); i++) {
		auto face = faces[i];
		CanvasTriangle triangle = CanvasTriangle();
		for (int j=0; j<face.vertices.size(); j++) {
			auto vertex = face.vertices[j] - camera;
			triangle.vertices[j] = CanvasPoint(
				round(-(planeMultiplier * focalLength * (vertex[0] / vertex[2])) + (window.width / 2)),
				round((planeMultiplier * focalLength * (vertex[1] / vertex[2])) + (window.height / 2)),
				vertex[2]
			);
		}

		drawFilledTriangle(window, depthBuffer, triangle, face.colour, false);
	}
}

void update(DrawingWindow &window) {
	// Function for performing animation (shifting artifacts or moving the camera)
}

void handleEvent(SDL_Event event, DrawingWindow &window, std::vector<float> &depthBuffer, TextureMap &texMap) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
		else if (event.key.keysym.sym == SDLK_c) window.clearPixels();
		else if (event.key.keysym.sym == SDLK_u) drawStrokedTriangle(window, depthBuffer, generateTriangle(window), Colour(rand()%256, rand()%256, rand()%256));
		else if (event.key.keysym.sym == SDLK_f) drawFilledTriangle(window, depthBuffer, generateTriangle(window), Colour(rand()%256, rand()%256, rand()%256), true);
		else if (event.key.keysym.sym == SDLK_t) drawTexturedTriangle(window, depthBuffer, generateTexturedTriangle(), texMap);
	} else if (event.type == SDL_MOUSEBUTTONDOWN) window.savePPM("output.ppm");
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	TextureMap texMap = TextureMap("texture.ppm");
	std::vector<float> depthBuffer = std::vector<float>(window.height * window.width, 0);
	
	float vertexScale = 1.0;

	std::vector<ModelTriangle> faces = loadObjFile("models/cornell-box.obj", vertexScale);
	glm::vec3 camera = glm::vec3(0.0, 0.0, 10.0);
	float focalLength = 5.0;



	SDL_Event event;
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) {
			handleEvent(event, window, depthBuffer, texMap);
			// update(window);
			draw(window, depthBuffer, camera, focalLength, faces);
		}
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
