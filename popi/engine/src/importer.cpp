#include <graphics.h>
#include <settings.h>
#include <components.h>
#include <importer.h>
#include  <vector>
#include <glad/gl.h>
#include <map>
#include <filesystem>
#include <utils.h>
#include <scene.h>
#include <json.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
namespace fs = std::filesystem;
using namespace PopiEngine::Graphics;
using std::string, std::vector, std::map, std::format;
using namespace PopiEngine::Logging;

/// <summary>
/// Importer handles finding paths to resources the engine will use.
/// It doesnt actually load any of the resources, just finds them and stores their paths.
/// </summary>
namespace PopiEngine::Importer
{

	map<string, vector<shaderPathDefinition>> shaderPaths = ImportShaders();
	map<string, texturePathDefinition> texturePaths = ImportTextures();
	map<string, meshPathDefinition> meshPaths = ImportMeshes();
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

	map<string, texturePathDefinition> ImportTextures() {

		map<string, texturePathDefinition> texturePaths;

		//Image file extensions
		const string validExtentions[] = { ".png", ".jpg"};

		std::cout << "Importing textures from " << RESOURCES_TEXTURES << std::endl;

		//Get file paths,and names of the textures. 
		for (const auto& p : fs::recursive_directory_iterator(RESOURCES_TEXTURES)) {
			string path = p.path().string();
			string ext = p.path().extension().string();
			string name = p.path().stem().string();
			for (const auto& validExt : validExtentions) {
			
				if (ext == validExt) {

					//If the extension is valid, add the texture to the map
					texturePathDefinition texturePathDef = { path };
					if (texturePaths.find(name) == texturePaths.end()) {
						//Texture name not in map, add it
						texturePaths[name] = texturePathDef;
						LogNormal(format("Texture Importer: Found texture: {} at path: {}", name, path));
					}
					else {
						LogWarning(format("Texture Importer: Texture {} already exists, skipping import.", name));
					}
				}
			}


		}

		return texturePaths;

	}

	map<string, meshPathDefinition> ImportMeshes()
	{
		map<string, meshPathDefinition> meshPaths;

		const string validExtentions[] = { ".obj"};

		std::cout << "Importing models from " << RESOURCES_MESHES << std::endl;

		//Get file paths,and names of the meshes. 
		for (const auto& p : fs::recursive_directory_iterator(RESOURCES_MESHES)) {
			string path = p.path().string();
			string ext = p.path().extension().string();
			string name = p.path().stem().string();
			for (const auto& validExt : validExtentions) {
				if (ext == validExt) {

					//If the extension is valid, add the mesh to the map
					meshPathDefinition meshPathDef = { path, ModelType::OBJ };
					if (meshPaths.find(name) == meshPaths.end()) {
						if(meshPathDef.type == ModelType::OBJ) {
							//Load the mesh using the OBJ loader
							ObjLoader(&meshPathDef);
						}
						else {
							LogError(format("Mesh Importer: Unsupported mesh type for {}.", name));
							continue;
						}
						LogNormal(format("Mesh Importer: Added mesh: {} at path: {}", name, path));
						meshPaths[name] = meshPathDef;
					}
					else {
						LogWarning(format("Texture Importer: Texture {} already exists, skipping import.", name));
					}
				}
			}
		}
		
		return meshPaths;
	}

	void ObjLoader(meshPathDefinition* meshDefinition) {
		LogNormal(format("Loading OBJ file from path: {}", meshDefinition->path));
		tinyobj::attrib_t attrib;
		vector<tinyobj::shape_t> shapes;
		vector<tinyobj::material_t> materials;
		string  error;
	
		bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &error, meshDefinition->path.c_str());

		if (!success) {
			LogError(format("Failed to load OBJ file: {}", error));
		} 

		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		//Thank you professor for the code êΩÇ…ä¥é”ÇµÇƒÇ®ÇËÇ‹Ç∑ÅB

		for (int s = 0; s < shapes.size(); s++) {
			 tinyobj::mesh_t& mesh = shapes[s].mesh;
			for (int i = 0; i < mesh.indices.size(); i++) {
				 tinyobj::index_t& index = mesh.indices[i];

				glm::vec3 position(0.0f, 0.0f, 0.0f);
				glm::vec3 normal(0.0f, 0.0f, 0.0f);
				glm::vec2 texcoord(0.0f, 0.0f); // Default texture coordinates
				if (index.vertex_index >= 0) {
					position =
						glm::vec3(attrib.vertices[index.vertex_index * 3 + 0], attrib.vertices[index.vertex_index * 3 + 1],
							attrib.vertices[index.vertex_index * 3 + 2]);
				}

				if (index.normal_index >= 0) {
					normal =
						glm::vec3(attrib.normals[index.normal_index * 3 + 0], attrib.normals[index.normal_index * 3 + 1],
							attrib.normals[index.normal_index * 3 + 2]);
				}
				if (index.texcoord_index >= 0) {
					texcoord.x = attrib.texcoords[2 * size_t(index.texcoord_index) + 0];
					texcoord.y = attrib.texcoords[2 * size_t(index.texcoord_index) + 1];
				}
	
				 Vertex vertex(position, normal, texcoord);

				indices.push_back(vertices.size());
				vertices.push_back(vertex);
			}
		}

		meshDefinition->indices = indices;
		meshDefinition->vertices = vertices;

	}
	
	
	void Scene::Load() {
		LogNormal("Loading scene...");
	}

	void Scene::Save() {

		LogNormal("Saving scene...");
	}

	//Placeholder (will implement later)
	json Scene::ToJson() {
		return json({
			{"entities", json::array()}
			});
	}

	void Scene::FromJson(const json& j) {

		LogNormal("Loading scene from JSON...");
	}

#pragma region Component Json Serializers
	void ToJson(json& j, const Transform& transform) {
	
	}
	void FromJson(const json& j, Transform& transform) {

	}

	void ToJson(json& j, const MeshRenderer& meshRenderer) {

	}
	void FromJson(const json& j, MeshRenderer& meshRenderer) {

	}

	void ToJson(json& j, const DirectionalLight& directionalLight) {

	}
	void FromJson(const json& j, DirectionalLight& directionalLight) {

	}

	void ToJson(json& j, const PointLight& pointLight) {

	}
	void FromJson(const json& j, PointLight& pointLight) {

	}

	void ToJson(json& j, const ECS::Camera& camera) {

	}
	void FromJson(const json& j, ECS::Camera& camera) {

	}
#pragma endregion

	
}