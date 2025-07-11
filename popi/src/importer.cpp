#include <graphics.h>
#include <settings.h>
#include  <vector>
#include <glad/gl.h>
#include <map>
#include <filesystem>
namespace fs = std::filesystem;
using namespace PopiEngine::Graphics;
using std::string, std::vector, std::map;


namespace PopiEngine::Importer
{
	/// <summary>
	/// Loads all shaders from the resources/shaders directory.
	/// 
	/// Since open 
	/// 
	/// </summary>
	/// <returns></returns>
	map<string, std::shared_ptr<Shader>> ImportShaders()
	{
		map<string, std::shared_ptr<Shader>> shaders;

		struct shaderData {
			string shaderPath;
			string shaderName;
			GLenum shaderType;
		};

		vector<shaderData> shadersData;

		const string fragmentExtn = ".frag";
		const string vertexExtn = ".vert";

		std::cout << "Importing shaders from " << RESOURCES_SHADERS << std::endl;
		//Get file paths,and names of the shaders. Then determine their type based on the extension.
		for (auto& p : fs::directory_iterator(RESOURCES_SHADERS)) {
			//Whyyy cant c# just let me switch on a string?!?
			string path = p.path().string();
			string ext = p.path().extension().string();
			string name = p.path().stem().string();

			if (p.path().extension() == fragmentExtn) {
				shadersData.push_back({ path,name, GL_FRAGMENT_SHADER });
			}
			else if (p.path().extension() == vertexExtn) {
				shadersData.push_back({ path,name, GL_VERTEX_SHADER });
			}



		}

		//Load shaders that compile successfully into the shader map
		for (int i = 0; i < shadersData.size(); i += 1)
		{
			try {
				std::cout << "Shader type for " << shadersData[i].shaderName << ": 0x"
					<< std::hex << shadersData[i].shaderType << std::endl;
				shaders[shadersData[i].shaderName] = std::make_shared<Shader>(shadersData[i].shaderType, shadersData[i].shaderPath);
			}
			catch (const std::exception& e) {
				std::cerr << "Error loading shader: " << shadersData[i].shaderName << " - " << e.what() << std::endl;
				continue; // Skip this shader if there's an error
			}
		}

		return shaders;

	}


}