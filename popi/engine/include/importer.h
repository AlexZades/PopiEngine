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
	struct shaderPathDefinition {
		string path;
		GLenum type;
	};

	extern map<string, vector<shaderPathDefinition>> shaderPaths;
	
	map<string, vector<shaderPathDefinition>> ImportShaders();
	
}