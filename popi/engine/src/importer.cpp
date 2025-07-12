#include <graphics.h>
#include <settings.h>
#include <importer.h>
#include  <vector>
#include <glad/gl.h>
#include <map>
#include <filesystem>
#include <utils.h>
namespace fs = std::filesystem;
using namespace PopiEngine::Graphics;
using std::string, std::vector, std::map, std::format;
using namespace PopiEngine::Logging;

/// <summary>
/// Importer helper functions for popi engine.
/// </summary>
namespace PopiEngine::Importer
{

	map<string, vector<shaderPathDefinition>> shaderPaths = ImportShaders();

	/// <summary>
	/// Loads shader paths from the resources/shaders directory.
	/// 
	/// Shader files that share the same name get added to the same vector and are evtually compiled to the same program.
	/// </summary>
	/// <returns></returns>
	/// 
	map<string, vector<shaderPathDefinition>> ImportShaders()
	{

		map<string, vector<shaderPathDefinition>> shaderPaths;

		//Shader file extensions
		const string fragmentExtn = ".frag";
		const string vertexExtn = ".vert";

		std::cout << "Importing shaders from " << RESOURCES_SHADERS << std::endl;
		//Get file paths,and names of the shaders. Then determine their type based on the extension.
		for (const auto& p : fs::recursive_directory_iterator(RESOURCES_SHADERS)) {

			//Whyyy cant c# just let me switch on a string?!?
			string path = p.path().string();
			string ext = p.path().extension().string();
			string name = p.path().stem().string();


			GLenum shaderType = 0; // 0 indicates no shader type found

			if (p.path().extension() == fragmentExtn) {
				shaderType = GL_FRAGMENT_SHADER;	
			}
			else if (p.path().extension() == vertexExtn) {
				shaderType = GL_VERTEX_SHADER;
			}
			shaderPathDefinition shaderPathDef = { path, shaderType };
			//Add shader to the map if it has a valid type
			if (shaderType != 0) {
				if (shaderPaths.find(name) == shaderPaths.end()) {
					//Shader name not in map, add it
					shaderPaths[name] = vector<shaderPathDefinition>();
					shaderPaths[name].push_back(shaderPathDef);
					LogNormal(format("Shader Importer: Found shader: {} at path: {}", name, path));
				}
				else {
					//Shader name already in map, append to existing vector
					shaderPaths[name].push_back(shaderPathDef);
					LogNormal(format("Shader Importer: Adding {} to shader {}", path, name));
				}
			}
			else {
				LogWarning(format("Invalid Shader File {}.", path));
			}

			
		}

		return shaderPaths;

	}


}