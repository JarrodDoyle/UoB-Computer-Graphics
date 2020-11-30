#pragma once

#include <vector>
#include "Colour.h"
#include "ModelTriangle.h"
#include "Utils.h"
#include <fstream>

std::vector<Colour> loadMtlFile(const std::string &filename);

std::vector<ModelTriangle> loadObjFile(const std::string &filename, float scale);