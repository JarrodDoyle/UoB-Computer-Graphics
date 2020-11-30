#include "OBJParser.h"

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

Model loadObjFile(const std::string &filename, float scale) {
	std::vector<glm::vec4> vertices;
	std::vector<TexturePoint> textureVertices;
	std::vector<glm::vec3> normals;
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
			vertices.push_back(glm::vec4(
				std::stof(vector[1]) * scale,
				std::stof(vector[2]) * scale,
				std::stof(vector[3]) * scale,
				1.0
			));
		}
		else if (vector[0] == "vt") {
			textureVertices.push_back(TexturePoint(
				round(std::stof(vector[1]) * 480),
				round(std::stof(vector[2]) * 395)
			));
		}
		else if (vector[0] == "vn") {
			normals.push_back(glm::vec3(
				std::stof(vector[1]),
				std::stof(vector[2]),
				std::stof(vector[3])
			));
		}
		else if (vector[0] == "f") {
			auto triangle = ModelTriangle();
			triangle.colour = colour;
			for (int i=0; i < 3; i++) {
				auto v = split(vector[i + 1], '/');
				triangle.vertices[i] = vertices[std::stoi(v[0]) - 1];
				if (v.size() > 1 && v[1] != "") triangle.texturePoints[i] = textureVertices[std::stoi(v[1]) - 1];
				if (v.size() > 2 && v[2] != "") triangle.vertexNormals[i] = normals[std::stoi(v[2]) - 1];
			}
			triangle.normal = glm::normalize(glm::cross(glm::vec3(triangle.vertices[1] - triangle.vertices[0]), glm::vec3(triangle.vertices[2] - triangle.vertices[0])));
			faces.push_back(triangle);
		}
	}
	return Model(filename, faces, materials);
}