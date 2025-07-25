#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <json.hpp>


using json = nlohmann::json;
using std::string, std::format, std::vector, std::shared_ptr;
/// <summary>
/// I wanted to make a simple ECS system for PopiEngine.
/// The way the system works is each entity can have multiple components.
/// Each component is a struct that contains data relevant to that component.
///  The components are stored in a bitmask, so we can easily check which components are active.
/// </summary>
namespace PopiEngine::ECS {

	enum ActiveComponents {
		TRANSFORM = 1 << 0, // 0001
		MESH_RENDERER = 1 << 1, // 0010
		DIRECTIONAL_LIGHT = 1 << 2, // 0100
		POINT_LIGHT = 1 << 3, // 1000
		PROCEDURAL_TERRAIN = 1 << 4, // 10000
		CAMERA = 1 << 5, // 100000
		SCRIPT_HOOK = 1 << 6 // 1000000
	};

	enum CameraMode {
		PERSPECTIVE,
		ORTHOGRAPHIC
	};

	struct Transform {
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

		
	};

	struct MeshRenderer {
		GLuint meshID;
		bool isTransparent = false; //we just use this for depth sorting
	
	
	};



	struct DirectionalLight {
		float intensity = 1.0f;
		glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

	
	};

	struct PointLight {
		float intensity = 1.0f; // Intensity of the light
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		float constant;
		float linear;
		float quadratic;

	
	};



	struct ProceduralTerrain {


	};

	struct Camera {
		glm::vec3 defaultUp = glm::vec3(0.0f, 1.0f, 0.0f);
		CameraMode mode = CameraMode::PERSPECTIVE;
		float fov = 70.0f; // Field of view for perspective camera
		float nearPlane = 0.1f;
		float farPlane = 100.0f;

		float orthographicSize = 10.0f;


		
	};

	struct ScriptHook {
		string scriptName = "";
		bool isActive = true; //If the script is active or not
	};
}
using namespace PopiEngine::ECS;
namespace PopiEngine::Importer {
#pragma region Component Json Serialziers
	void ToJson(json& j,  Transform& transform);
	void FromJson(const json& j, Transform& transform);
	
	void ToJson(json& j,  DirectionalLight& directionalLight);
	void FromJson(const json& j, DirectionalLight& directionalLight);

	void ToJson(json& j,  PointLight& pointLight);
	void FromJson(const json& j, PointLight& pointLight);

	void ToJson(json& j,  MeshRenderer& meshRenderer);
	void FromJson(const json& j, MeshRenderer& meshRenderer);


	void ToJson(json& j,  ECS::Camera& camera);
	void FromJson(const json& j, ECS::Camera& camera);

	void ToJson(json& j, ScriptHook& scriptHook);
	void FromJson(const json& j, ScriptHook& scriptHook);

#pragma endregion


}



