#pragma once

#include <vector>
#include "Colour.h"
#include "Model.h"
#include "Utils.h"
#include <fstream>

std::vector<Colour> loadMtlFile(const std::string &filename);

Model loadObjFile(const std::string &filename, float scale);