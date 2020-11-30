#pragma once

#include <vector>
#include "Material.h"
#include "Model.h"
#include "Utils.h"
#include <fstream>

std::vector<Material> loadMtlFile(const std::string &filename);

Model loadObjFile(const std::string &filename, float scale);