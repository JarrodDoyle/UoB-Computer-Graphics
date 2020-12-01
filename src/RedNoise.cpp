#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <CanvasTriangle.h>
#include <Colour.h>
#include <ModelTriangle.h>
#include <Model.h>
#include <TextureMap.h>
#include <TexturePoint.h>
#include <algorithm>
#include <Maths.h>
#include <RayTriangleIntersection.h>
#include <cmath>
#include <OBJParser.h>
#include <Light.h>

#define WIDTH 640
#define HEIGHT 480

#define PI 3.14159265358979323846

RayTriangleIntersection getClosestIntersection(glm::vec3 startPoint, glm::vec3 direction, std::vector<Model> &models) {
	RayTriangleIntersection intersection;
	float epsilon = 0.0001;
	intersection.distanceFromCamera = std::numeric_limits<float>::infinity();
	for (const auto& model : models) {
		for (const auto& triangle : model.faces) {
			auto point = glm::inverse(glm::mat3(
				-direction,
				glm::vec3(triangle.vertices[1] - triangle.vertices[0]),
				glm::vec3(triangle.vertices[2] - triangle.vertices[0])
			)) * (startPoint - glm::vec3(triangle.vertices[0]));

			if (epsilon <= point[0] && point[0] < intersection.distanceFromCamera &&
				0 <= point[1] && point[1] <= 1 &&
				0 <= point[2] && point[2] <= 1 &&
				point[1] + point[2] <= 1
			) {
				intersection.intersectionPoint = point;
				intersection.distanceFromCamera = point[0];
				intersection.intersectedTriangle = triangle;
			}
		}
	}
	return intersection;
}

float getNumberOfSteps(CanvasPoint from, CanvasPoint to) {
	return fmax(fmax(abs(to.x - from.x), abs(to.y - from.y)), 1);
}

void sortTriangleVertices(CanvasTriangle &triangle) {
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

std::vector<TexturePoint> interpolatePoints(TexturePoint from, TexturePoint to, int steps) {
	std::vector<TexturePoint> TexturePoints;
	float xStep = (to.x - from.x) / (steps - 1);
	float yStep = (to.y - from.y) / (steps - 1);
	for (int i=0; i<steps; i++) {
		TexturePoints.push_back(TexturePoint(round(from.x + (i * xStep)),  round(from.y + (i * yStep))));
	}
	return TexturePoints;
}

std::vector<CanvasPoint> interpolatePoints(CanvasPoint from, CanvasPoint to, int steps) {
	std::vector<CanvasPoint> canvasPoints;
	float xStep = (to.x - from.x) / (steps - 1);
	float yStep = (to.y - from.y) / (steps - 1);
	float dStep = (to.depth - from.depth) / (steps - 1);
	auto texturePoints = interpolatePoints(from.texturePoint, to.texturePoint, steps);
	for (int i=0; i<steps; i++) {
		CanvasPoint point(round(from.x + (i * xStep)),  round(from.y + (i * yStep)), from.depth + (i * dStep));
		point.texturePoint = texturePoints[i];
		canvasPoints.push_back(point);
	}
	return canvasPoints;
}

void drawLine(DrawingWindow &window, std::vector<float> &depthBuffer, CanvasPoint from, CanvasPoint to, float numberOfSteps, std::vector<Colour> colours) {
	auto points = interpolatePoints(from, to, numberOfSteps + 1);
	for (float i=0.0; i<=numberOfSteps; i++) {
		auto x = points[i].x;
		auto y = points[i].y;
		if (0 <= y && y < window.height && 0 <= x && x < window.width) {
			int depthIndex = (y * window.width) + x;
			if (points[i].depth > depthBuffer[depthIndex]) {
				depthBuffer[depthIndex] = points[i].depth;
				uint32_t c = (255 << 24) + (colours[i].red << 16) + (colours[i].green << 8) + colours[i].blue;
				window.setPixelColour(x, y, c);
			}
		}
	}
}

void drawLine(DrawingWindow &window, std::vector<float> &depthBuffer, CanvasPoint from, CanvasPoint to, Colour colour) {
	float numberOfSteps = getNumberOfSteps(from, to);
	drawLine(window, depthBuffer, from, to, numberOfSteps, std::vector<Colour>(numberOfSteps + 1, colour));
}

template <typename T>
void getRowStartsAndEnds(T p1, T p2, T p3, float height1, float height2, std::vector<T> &rowStarts, std::vector<T> &rowEnds) {
	rowStarts = interpolatePoints(p1, p2, height1);
	if (height2 > 1) {
		auto side2 = interpolatePoints(p2, p3, height2);
		rowStarts.pop_back();
		rowStarts.insert(rowStarts.end(), side2.begin(), side2.end());
	}
	rowEnds = interpolatePoints(p1, p3, height1 + height2 - 1);
}

template <typename T>
void getRowStartsAndEnds(CanvasTriangle triangle, float height1, float height2, std::vector<T> &rowStarts, std::vector<T> &rowEnds) {
	getRowStartsAndEnds(triangle[0], triangle[1], triangle[2], height1, height2, rowStarts, rowEnds);
}

CanvasTriangle generateTriangle(DrawingWindow &window) {
	float depth = (float(rand()) / float(RAND_MAX)) * -10;
	return CanvasTriangle(
		CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1), depth),
		CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1), depth),
		CanvasPoint(rand()%(window.width - 1), rand()%(window.height - 1), depth)
	);
}

CanvasTriangle generateTexturedTriangle() {
	float depth = (float(rand()) / float(RAND_MAX)) * -10;
	CanvasTriangle triangle = CanvasTriangle(CanvasPoint(160, 10, depth), CanvasPoint(300, 230, depth), CanvasPoint(10, 150, depth));
	triangle[0].texturePoint = TexturePoint(195, 5);
	triangle[1].texturePoint = TexturePoint(395, 380);
	triangle[2].texturePoint = TexturePoint(65, 330);
	return triangle;
}

void drawStrokedTriangle(DrawingWindow &window, std::vector<float> &depthBuffer, CanvasTriangle triangle, Colour colour) {
	drawLine(window, depthBuffer, triangle[0], triangle[1], colour);
	drawLine(window, depthBuffer, triangle[0], triangle[2], colour);
	drawLine(window, depthBuffer, triangle[1], triangle[2], colour);
}

void drawFilledTriangle(DrawingWindow &window, std::vector<float> &depthBuffer, CanvasTriangle triangle, Colour colour, bool outline) {
	sortTriangleVertices(triangle);

	// Work out the start and end xPos of each row of the triangle
	std::vector<CanvasPoint> starts, ends;
	getRowStartsAndEnds(triangle, triangle[1].y - triangle[0].y + 1, triangle[2].y - triangle[1].y + 1, starts, ends);

	// Draw the outline (if there is one) then fill in the triangle
	if (outline) drawStrokedTriangle(window, depthBuffer, triangle, Colour(255, 255, 255));

	for (int i=0; i<=triangle[2].y - triangle[0].y; i++) {
		starts[i].x = std::max(starts[i].x, 0.0f);
		ends[i].x = std::min(ends[i].x, float(window.width));
		drawLine(window, depthBuffer, starts[i], ends[i], colour);
	}
}

void drawTexturedTriangle(DrawingWindow &window, std::vector<float> &depthBuffer, CanvasTriangle triangle, TextureMap &texMap, bool outline) {
	sortTriangleVertices(triangle);

	float height1 = triangle[1].y - triangle[0].y + 1;
	float height2 = triangle[2].y - triangle[1].y + 1;
	std::vector<CanvasPoint> canvasStarts, canvasEnds;
	getRowStartsAndEnds(triangle, height1, height2, canvasStarts, canvasEnds);

	if (outline) drawStrokedTriangle(window, depthBuffer, triangle, Colour(255, 255, 255));

	for(int i=0; i<=height1 + height2 - 2; i++) {
		float numberOfSteps = getNumberOfSteps(canvasStarts[i], canvasEnds[i]);
		auto points = interpolatePoints(canvasStarts[i].texturePoint, canvasEnds[i].texturePoint, numberOfSteps + 1);

		std::vector<Colour> colours;
		for (float i=0.0; i<=numberOfSteps; i++) {
			uint32_t c = texMap.pixels[(int(round(points[i].y)) % texMap.height) * texMap.width + int(round(points[i].x)) % texMap.width];
			colours.push_back(Colour((c & 0xFF0000) >> 16, (c & 0xFF00) >> 8, (c & 0xFF)));
		}
		drawLine(window, depthBuffer, canvasStarts[i], canvasEnds[i], numberOfSteps, colours);
	}
}

void draw(DrawingWindow &window, int renderMode, glm::mat4 camera, float focalLength, float planeMultiplier, std::vector<Model> &models) {
	window.clearPixels();
	std::vector<float> depthBuffer = std::vector<float>(window.height * window.width, 0);

	for (int j=0; j<models.size(); j++) {
		auto faces = models[j].faces;
		for (int i=0; i<faces.size(); i++) {
			auto face = faces[i];
			CanvasTriangle triangle = CanvasTriangle();

			glm::vec4 camPos(camera[3]);
			glm::mat4 camOri(
				glm::vec4(camera[0]),
				glm::vec4(camera[1]),
				glm::vec4(camera[2]),
				glm::vec4(0.0, 0.0, 0.0, 1.0)
			);
			
			for (int j=0; j<face.vertices.size(); j++) {
				auto vertex = (face.vertices[j] -  camPos) * camOri;
				triangle.vertices[j] = CanvasPoint(
					round(-(planeMultiplier * focalLength * vertex[0] / vertex[2]) + (window.width / 2)),
					round((planeMultiplier * focalLength * vertex[1] / vertex[2]) + (window.height / 2)),
					-1/vertex[2]
				);
				triangle.vertices[j].texturePoint = face.texturePoints[j];
			}

			if (renderMode == 0) {
				drawStrokedTriangle(window, depthBuffer, triangle, face.material.colour);
			}
			else if (face.material.textureMap.width != 0) {
				drawTexturedTriangle(window, depthBuffer, triangle, face.material.textureMap, false);
			}
			else {
				drawFilledTriangle(window, depthBuffer, triangle, face.material.colour, false);
			}
		}
	}
	
}

float getPixelBrightness(glm::vec3 camDir, float u, float v, ModelTriangle triangle, std::vector<Model> &models, std::vector<Light> &lights, float specularScale, float ambientLight) {
	auto ps = triangle.vertices;
	auto r = glm::vec3(ps[0] + u * (ps[1] - ps[0]) + v * (ps[2] - ps[0]));
	float brightness = 0.0;
	for (const auto &light : lights) {
		
		std::vector<glm::vec3> directions;
		directions.push_back(light.position - r);
		// Poor attempt at softened shadows lol
		// for (int i=0; i<light.size * 2; i++) {
		// 	auto j = (i + 1) * 0.05;
		// 	directions.push_back(light.position + glm::vec3(-j, -j, j) - r);
		// 	directions.push_back(light.position + glm::vec3(-j, -j, -j) - r);
		// 	directions.push_back(light.position + glm::vec3(-j, j, j) - r);
		// 	directions.push_back(light.position + glm::vec3(-j, j, -j) - r);
		// 	directions.push_back(light.position + glm::vec3(j, -j, j) - r);
		// 	directions.push_back(light.position + glm::vec3(j, -j, -j) - r);
		// 	directions.push_back(light.position + glm::vec3(j, j, j) - r);
		// 	directions.push_back(light.position + glm::vec3(j, j, -j) - r);
		// }
		auto lightStrength = light.strength / directions.size();
		for (const auto &direction : directions) {
			auto intersect = getClosestIntersection(r, glm::normalize(direction), models);
			auto length = glm::length(direction);
			if (intersect.distanceFromCamera < length) continue;
			
			auto normals = triangle.vertexNormals;
			auto normal = (1 - u - v) * normals[0] + u * normals[1] + v * normals[2];
			if (normal[0] == 0 && normal[1] == 0 && normal[2] == 0) normal = triangle.normal;
			float proximity = std::min(1.0, lightStrength / ( 4 * PI * std::pow(length, 2)));
			float incidence = std::max(0.0f, glm::dot(glm::normalize(direction), normal));
			float specular = std::pow(glm::dot(glm::normalize(camDir), glm::normalize(direction - normal * 2.0f * glm::dot(direction, normal))), specularScale);
			brightness += std::max(specular, proximity * incidence);
			if (brightness >= 1.0) {
				brightness = 1.0;
				break;
			}
		}		
	}
	
	return std::max(ambientLight, brightness);
}

void drawRaytraced(DrawingWindow &window, glm::mat4 camera, float focalLength, float planeMultiplier, std::vector<Model> &models, std::vector<Light> &lights) {
	glm::vec3 camPos(camera[3]);
	glm::mat3 camOri(
		(glm::vec3(camera[0])),
		glm::vec3(camera[1]),
		glm::vec3(camera[2])
	);

	for (int x=0; x<window.width; x++) {
		for (int y=0; y<window.height; y++) {
			glm::vec3 direction(-(float(window.width / 2) - x) / planeMultiplier, (float(window.height / 2) - y) / planeMultiplier, -2);
			glm::vec3 camDir = glm::normalize(camOri * direction);
			auto intersect = getClosestIntersection(glm::vec3(camPos), camDir, models);
			auto colour = intersect.intersectedTriangle.material.colour;
			
			if (intersect.distanceFromCamera != std::numeric_limits<float>::infinity()) {
				auto tri = intersect.intersectedTriangle;
				auto u = intersect.intersectionPoint[1];
				auto v = intersect.intersectionPoint[2];
				if (tri.material.textureMap.width != 0) {
					auto w = tri.material.textureMap.width;
					auto h = tri.material.textureMap.height;
					int x = int((1 - u - v) * tri.texturePoints[0].x + u * tri.texturePoints[1].x + v * tri.texturePoints[2].x) % w;
					int y = int((1 - u - v) * tri.texturePoints[0].y + u * tri.texturePoints[1].y + v * tri.texturePoints[2].y) % h;
					uint32_t tColour = tri.material.textureMap.pixels[w * y + x];
					colour = Colour(tColour >> 16 & 0xFF, tColour >> 8 & 0xFF, tColour & 0xFF);
				}
				auto brightness = getPixelBrightness(camDir, u, v, tri, models, lights, 256, 0.1);
				colour.red *= brightness;
				colour.green *= brightness;
				colour.blue *= brightness;
			}
			uint32_t c = (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;
			window.setPixelColour(x, y, c);
		}
	}
}

void handleEvent(SDL_Event event, DrawingWindow &window, glm::mat4 &camera, int &renderMode) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_w) camera = translationMatrix(glm::vec3(0.0, 0.0, -0.5)) * camera;
		else if (event.key.keysym.sym == SDLK_s) camera = translationMatrix(glm::vec3(0.0, 0.0, 0.5)) * camera;
		else if (event.key.keysym.sym == SDLK_z) camera = translationMatrix(glm::vec3(0.5, 0.0, 0.0)) * camera;
		else if (event.key.keysym.sym == SDLK_c) camera = translationMatrix(glm::vec3(-0.5, 0.0, 0.0)) * camera;
		else if (event.key.keysym.sym == SDLK_q) camera = translationMatrix(glm::vec3(0.0, 0.5, 0.0)) * camera;
		else if (event.key.keysym.sym == SDLK_e) camera = translationMatrix(glm::vec3(0.0, -0.5, 0.0)) * camera;
		else if (event.key.keysym.sym == SDLK_a) camera = rotationMatrixY(0.1) * camera;
		else if (event.key.keysym.sym == SDLK_d) camera = rotationMatrixY(-0.1) * camera;
		else if (event.key.keysym.sym == SDLK_r) camera = rotationMatrixX(-0.1) * camera;
		else if (event.key.keysym.sym == SDLK_v) camera = rotationMatrixX(0.1) * camera;
		else if (event.key.keysym.sym == SDLK_1) camera = rotationMatrixZ(0.1) * camera;
		else if (event.key.keysym.sym == SDLK_3) camera = rotationMatrixZ(-0.1) * camera;
		else if (event.key.keysym.sym == SDLK_l) camera = lookAt(camera, glm::vec3(0.0, 0.0, 0.0));
		else if (event.key.keysym.sym == SDLK_b) renderMode = 0;
		else if (event.key.keysym.sym == SDLK_n) renderMode = 1;
		else if (event.key.keysym.sym == SDLK_m) renderMode = 2;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) window.savePPM("output.ppm");
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	
	std::vector<Model> models;
	models.push_back(loadObjFile("models/textured-cornell-box.obj", 1.0));
	models.push_back(loadObjFile("models/sphere.obj", 1.0));
	models.push_back(loadObjFile("models/logo.obj", 0.01));

	std::vector<Light> lights;
	lights.push_back(Light(glm::vec3(1.0, 2.0, 4.0), 1, 50));
	lights.push_back(Light(glm::vec3(1.0, 0.0, 0.0), 1, 50));
	glm::mat4 camera(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 10.0, 1.0
	);
	float focalLength = 2.0;
	float planeMultiplier = 250;
	int renderMode = 1;

	SDL_Event event;
	while (true) {
		if (window.pollForInputEvents(event)) {
			handleEvent(event, window, camera, renderMode);
			if (renderMode == 2) drawRaytraced(window, camera, focalLength, planeMultiplier, models, lights);
			else draw(window, renderMode, camera, focalLength, planeMultiplier, models);
		}
		window.renderFrame();
	}
}
