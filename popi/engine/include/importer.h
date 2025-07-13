#pragma once
#include <graphics.h>
#include <settings.h>
#include  <vector>
#include <map>
#include <filesystem>
#include <glad/gl.h>
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

	struct texturePathDefinition {
		string path;
	};
	//For use later when I implement assimp
	enum ModelType {
		OBJ,
		FBX
	};

	struct meshPathDefinition {
		string path;
		ModelType type;
		vector<Vertex> vertices;
		vector<GLuint> indices;
	
	};



	extern map<string, vector<shaderPathDefinition>> shaderPaths;

	extern map<string, texturePathDefinition> texturePaths;

	extern map<string, meshPathDefinition> meshPaths;

	void ObjLoader(meshPathDefinition* meshDefinition);

	map<string, vector<shaderPathDefinition>> ImportShaders();

	map<string, texturePathDefinition> ImportTextures();
	
	map<string, meshPathDefinition> ImportMeshes();
}