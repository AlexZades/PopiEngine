#pragma once

#include <graphics.h>
#include <settings.h>
#include  <vector>
#include <map>
#include <filesystem>
namespace fs = std::filesystem;

using std::string, std::vector, std::map;
#pragma once
using namespace PopiEngine::Graphics;


namespace PopiEngine::Importer
{
	map<string, std::shared_ptr<Shader>> ImportShaders();

	
}