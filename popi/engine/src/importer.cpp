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
#include <project.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
namespace fs = std::filesystem;
using namespace PopiEngine::Graphics;
using std::string, std::vector, std::map, std::format;
using namespace PopiEngine::Logging;
using json = nlohmann::json;
/// <summary>
/// Importer handles finding paths to resources the engine will use.
/// It doesnt actually load any of the resources, just finds them and stores their paths.
/// </summary>
namespace PopiEngine::Importer
{
	

	map<string, vector<shaderPathDefinition>> shaderPaths = ImportShaders();
	map<string, texturePathDefinition> texturePaths = ImportTextures();
	map<string, meshPathDefinition> meshPaths = ImportMeshes();
	map<string, scenePathDefinition> scenePaths = ImportScenes();
	Scene currentScene; //This is bad fix later 
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

		LogNormal(format("Importing shaders from {}", RESOURCES_SHADERS));
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

		LogNormal(format("Importing textures from {}", RESOURCES_TEXTURES));

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

		LogNormal(format("Importing meshes from {} " ,RESOURCES_MESHES));

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
						LogWarning(format("Mesh {} already exists, skipping import.", name));
					}
				}
			}
		}
		
		return meshPaths;
	}

	map<string, scenePathDefinition> ImportScenes()
	{
		map<string, scenePathDefinition> scenes;

		const string validExtentions[] = { ".popisc" };

		std::cout << "Importing scenes from " << RESOURCES_SCENES << std::endl;

		//Get file paths,and names of the meshes. 
		for (const auto& p : fs::recursive_directory_iterator(RESOURCES_SCENES)) {
			string path = p.path().string();
			string ext = p.path().extension().string();
			string name = p.path().stem().string();
			for (const auto& validExt : validExtentions) {
				if (ext == validExt) {
					//If the extension is valid, add the mesh to the map
					scenePathDefinition scenePathDef = { path };
					if (scenes.find(name) == scenes.end()) {
						scenes[name] = scenePathDef;
					}
					else {
						LogWarning(format("Scene Importer: Scene {} already exists, skipping import.", name));
					}
				}
			}
		}
		return scenes;
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
		//Thank you professor for the code 誠に感謝しております。

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
	
	
	void Scene::Load(string path) {
		entityManagerRef->OnSceneLoad();
		LogNormal(format("Loading scene from path: {}", path));
		std::ifstream file(path);
		if (!file.is_open()) {
			LogError(format("Failed to open scene file: {}", path));
			return;
		}
		json sceneJson;
		try {
			file >> sceneJson;
			file.close();
			fromJson(sceneJson);
			LogNormal(format("Scene {} loaded successfully.", name));
		}
		catch (const std::exception& e) {
			LogError(format("Failed to load scene from file {}: {}", path, e.what()));
			return;
		}
		LogNormal("Loading scene...");
	}

	void Scene::New()
	{
	this->sceneEntities = entities;
	LogNormal("Creating new scene...");
	entityManagerRef->OnNewScene();
	name = "scene";
	LogNormal("New scene created successfully.");
	}

	void Scene::Save() {
		this->sceneEntities = entities;
		json sceneJson = toJson();	
		string scenePath = format("{}/{}.popisc", RESOURCES_SCENES, name);
		LogNormal("Saving scene...");
		std::ofstream file(scenePath);
		if (file.is_open()) {
			file << sceneJson.dump(4); 
			file.close();
			LogNormal(format("Scene {} saved to {}", name, scenePath));
		}
		else {
			LogError(format("Failed to save scene to {}", scenePath));
		}

		LogNormal("Scene saved successfully. Refreshng resources");
		scenePaths = ImportScenes(); //Refresh the scene paths after saving
	}

	json Scene::toJson() {
    json j;  
    
    j["format version"] = SCENE_VERSION;  
    j["scene"]["name"] = name;  
    vector<json> entitiesJson;

    if (!entities.empty()) {  
        // Serialize each entity 
        for (const auto& entity : sceneEntities) {
            json entityJson;  
            entityJson["name"] = entity->name; 
            LogNormal(format("Serializing entity: {}", entity->name));

            ActiveComponents activeComponents = entity->GetActiveComponents();
            
            // Check each component using bitwise AND operations (active components is a bit mask)
            if (activeComponents & ActiveComponents::TRANSFORM) {
                std::shared_ptr<Transform> transform = entity->transform;	
                if (transform != nullptr) {
                    json transformJson;
                    ToJson(transformJson, *transform.get());
                    entityJson["transform"] = transformJson;
                    LogNormal(format("Serialized transform for entity: {}", entity->name));
                } 
				else {
                    LogWarning(format("Entity {} has TRANSFORM flag but no transform component.", entity->name));
                }
            }
            
            if (activeComponents & ActiveComponents::MESH_RENDERER) {
                std::shared_ptr<MeshRenderer> meshRenderer = entity->meshRenderer;
                if (meshRenderer != nullptr) { 
                    json meshRendererJson;
                    ToJson(meshRendererJson, *meshRenderer.get());
                    entityJson["mesh_renderer"] = meshRendererJson;
                    LogNormal(format("Serialized mesh renderer for entity: {}", entity->name));
                } 
				else {
                    LogWarning(format("Entity {} has MESH_RENDERER flag but no mesh renderer component.", entity->name));
                }
            }
            
            /*
            if (activeComponents & ActiveComponents::DIRECTIONAL_LIGHT) {
                std::shared_ptr<DirectionalLight> directionalLight = entity->directionalLight;
                if (directionalLight != nullptr) { 
                    json directionalLightJson;
                    ToJson(directionalLightJson, *directionalLight.get());
                    entityJson["directional_light"] = directionalLightJson;
                    LogNormal(format("Serialized directional light for entity: {}", entity->name));
                } 
				else {
                    LogWarning(format("Entity {} has DIRECTIONAL_LIGHT flag but no directional light component.", entity->name));
                }
            }
            
            if (activeComponents & ActiveComponents::POINT_LIGHT) {
                std::shared_ptr<PointLight> pointLight = entity->pointLight;
                if (pointLight != nullptr) {
                    json pointLightJson;
                    ToJson(pointLightJson, *pointLight.get());
                    entityJson["point_light"] = pointLightJson;
                    LogNormal(format("Serialized point light for entity: {}", entity->name));
                } 
				else {
                    LogWarning(format("Entity {} has POINT_LIGHT flag but no point light component.", entity->name));
                }
            }
            */
			if (activeComponents & ActiveComponents::CAMERA) {
				std::shared_ptr<ECS::Camera> cam = entity->camera;
				if (cam != nullptr) {
					json cameraJson;
					ToJson(cameraJson, *cam.get());
					entityJson["camera"] = cameraJson;
					LogNormal(format("Serialized camera for entity: {}", entity->name));
				}
				else {
					LogWarning(format("Entity {} has CAMERA flag but no camera component.", entity->name));
				}
			}

			if (activeComponents & ActiveComponents::SCRIPT_HOOK) {
				std::shared_ptr<ScriptHook> scriptHook = entity->scriptHook;
				if (scriptHook != nullptr) {
					json scriptHookJson;
					ToJson(scriptHookJson, *scriptHook.get());
					entityJson["script_hook"] = scriptHookJson;
					LogNormal(format("Serialized script hook for entity: {}", entity->name));
				}
				else {
					LogWarning(format("Entity {} has SCRIPT_HOOK flag but no script hook component.", entity->name));
				}
			}

            entitiesJson.push_back(entityJson);
        }  
        j["scene"]["entities"] = entitiesJson;
    }  
    
    return j;
}

	void Scene::fromJson(const json& j) {
		int version = j["format version"];
		if (version != SCENE_VERSION) {
			LogError(format("Scene file is outdated (ver {}, may not load correctly", version));
		}
		LogNormal("Loading scene from JSON...");

		name = j["scene"]["name"].get<string>();
		LogNormal(format("Loaing Scene name: {}", name));
		vector<json> entitiesJson = j["scene"]["entities"].get<vector<json>>();

		for (const auto& entityJson : entitiesJson) {
			
			LogNormal(format("Deserializing entity: {}", entityJson.dump(4)));

			string ename = entityJson["name"].get<string>();

            auto entity = entityManagerRef.get()->InstatiateEntity(ename);
			if (entityJson.contains("transform")&& entityJson["transform"].is_object()) {
				LogNormal(format("Deserializing transform for entity: {}", ename));
				Transform transform;
				FromJson(entityJson["transform"], transform);
				entity->AttachTransform(std::make_shared<Transform>(transform));
			}
			if (entityJson.contains("mesh_renderer") && entityJson["mesh_renderer"].is_object()) {
				LogNormal(format("Deserializing mesh renderer for entity: {}", ename));
				MeshRenderer meshRenderer;
				FromJson(entityJson["mesh_renderer"], meshRenderer);
				entity->AttachMesh(std::make_shared<MeshRenderer>(meshRenderer));
			}
			if (entityJson.contains("directional_light") && entityJson["directional_light"].is_object()) {
				LogNormal(format("Deserializing directional light for entity: {}", ename));
				DirectionalLight directionalLight;
				FromJson(entityJson["directional_light"], directionalLight);
				entity->AttachDirectionalLight(std::make_shared<DirectionalLight>(directionalLight));
			}
			if (entityJson.contains("point_light") && entityJson["point_light"].is_object()) {
				LogNormal(format("Deserializing point light for entity: {}", ename));
				PointLight pointLight;
				FromJson(entityJson["point_light"], pointLight);
				entity->AttachPointLight(std::make_shared<PointLight>(pointLight));
			}
			if (entityJson.contains("camera") && entityJson["camera"].is_object()) {
				LogNormal(format("Deserializing camera for entity: {}", ename));
				ECS::Camera camera;
				FromJson(entityJson["camera"], camera);
				entity->AttachCamera(std::make_shared<ECS::Camera>(camera));
			}
			if (entityJson.contains("script_hook") && entityJson["script_hook"].is_object()) {
				LogNormal(format("Deserializing script hook for entity: {}", ename));
				ScriptHook scriptHook;
				FromJson(entityJson["script_hook"], scriptHook);
				entity->AttachScriptHook(std::make_shared<ScriptHook>(scriptHook));
			}
			else {
				LogWarning(format("Entity {} has no components.", ename));
			}
		}

	}

#pragma region Component Json Serializers

	void ToJson(json& j, Transform& transform) {
		j = json{
			{"position", {transform.position.x, transform.position.y, transform.position.z}},
			{"rotation", {transform.rotation.x, transform.rotation.y, transform.rotation.z}},
			{"scale", {transform.scale.x, transform.scale.y, transform.scale.z}}
		};
	}
	void FromJson(const json& j, Transform& transform) {
		transform.position = glm::vec3(j["position"][0], j["position"][1], j["position"][2]);
		transform.rotation = glm::vec3(j["rotation"][0], j["rotation"][1], j["rotation"][2]);
		transform.scale = glm::vec3(j["scale"][0], j["scale"][1], j["scale"][2]);
	}
	
	void ToJson(json& j,  MeshRenderer &meshRenderer) {
		string meshName = "";
		string shaderProgramName = UNLIT_SHADER;
		LogNormal(shaderProgramName);
		//Maybe rewrite this to use a struct to handle the texture data
		vector<string> textureNames;
		vector<TextureType> textureTypes;
		json materialJson;
		auto& mesh = activeGraphicsCore->activeMeshes[meshRenderer.meshID];

		if (mesh) {
			meshName = mesh.get()->name;

			//we need to fix this by storing the shader name on the mesh
			//shaderProgramName = "shaderPrograms.."; //Lol c++ 
			for(const auto& texture : mesh.get()->textures) {
				textureNames.push_back(texture.name);
				textureTypes.push_back(texture.type);
			}
			auto material = mesh.get()->material;	
			materialJson =
			json{
				{"diffuse", {material.diffuse.r, material.diffuse.g, material.diffuse.b}},
				{"specular", {material.specular.r, material.specular.g, material.specular.b}},
				{"shininess", material.shininess}
			};
		}
		j = json{
			{"name", meshName},
			{"shaderProgramName", shaderProgramName},
			{"textures", textureNames},
			{"textureTypes", textureTypes},
			{"material", materialJson},
			{ "isTransparent", meshRenderer.isTransparent } 
		};
	}
	void FromJson(const json& j, MeshRenderer& meshRenderer) {
		string meshName = j["name"];
		string shaderProgramName = j["shaderProgramName"];
		
		vector<string> textureNames = j["textures"].get<vector<string>>();
		vector<TextureType> textureTypes = j["textureTypes"].get<vector<TextureType>>();
		//We rebuild the textures
		vector<Texture> textures;
		for (int i = 0; i < textureNames.size(); i++) {
			
			if (texturePaths.find(textureNames[i]) != texturePaths.end()) {
				textures.push_back(Texture(textureNames[i], textureTypes[i]));
			}
			else {
				LogWarning(format("Texture {} not found. Skipping", textureNames[i]));
			}
		}

		//Then create and link the mesh
		auto mesh = activeGraphicsCore->LinkMesh(std::make_shared<Mesh>(meshName,textures, shaderProgramName));
		try {
			Material material;
			material.diffuse = glm::vec3(j["material"]["diffuse"][0], j["material"]["diffuse"][1], j["material"]["diffuse"][2]);
			material.specular = glm::vec3(j["material"]["specular"][0], j["material"]["specular"][1], j["material"]["specular"][2]);
			material.shininess = j["material"]["shininess"];
			activeGraphicsCore->activeMeshes[mesh]->SetMaterial(material);
		}
		catch (const std::exception& e) {
			LogError(format("Failed to deserialize material for mesh renderer: {}", e.what()));
			return;
		}

		meshRenderer.meshID = mesh;
		meshRenderer.isTransparent = j["isTransparent"];
	}

	void ToJson(json& j, const DirectionalLight& directionalLight) {

	}
	void FromJson(const json& j, DirectionalLight& directionalLight) {
		
	}

	void ToJson(json& j, const PointLight& pointLight) {

	}
	void FromJson(const json& j, PointLight& pointLight) {

	}

	void ToJson(json& j,  ECS::Camera& camera) {
		j = json{
			{"defaultUp", {camera.defaultUp.x, camera.defaultUp.y, camera.defaultUp.z}},
			{"mode", static_cast<int>(camera.mode)},
			{"fov", camera.fov},
			{"nearPlane", camera.nearPlane},
			{"farPlane", camera.farPlane},
			{"orthographicSize", camera.orthographicSize}
		};
	}
	void FromJson(const json& j, ECS::Camera& camera) {
		camera.defaultUp = glm::vec3(j["defaultUp"][0], j["defaultUp"][1], j["defaultUp"][2]);
		camera.mode = static_cast<CameraMode>(j["mode"].get<int>());
		camera.fov = j["fov"];
		camera.nearPlane = j["nearPlane"];
		camera.farPlane = j["farPlane"];
		camera.orthographicSize = j["orthographicSize"];
	}
	void ToJson(json& j, ScriptHook& scriptHook)
	{
		j = json{
			{"scriptName", scriptHook.scriptName},
			{"isActive", scriptHook.isActive}
		};
		LogNormal(format("Serializing ScriptHook: {}", scriptHook.scriptName));
	}
	void FromJson(const json& j, ScriptHook& scriptHook)
	{
		scriptHook.scriptName = j["scriptName"].get<string>();
		scriptHook.isActive = j["isActive"].get<bool>();
		LogNormal(format("Deserializing ScriptHook: {}", scriptHook.scriptName));
	}
#pragma endregion

	
}