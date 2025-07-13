#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

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
		DIRECTIONAL_LIGHT = 1 << 2 // 0100
	};

	struct Transform {
		glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f);
		glm::vec3 rotation = glm::vec3(0.0f,0.0f,0.0f);
		glm::vec3 scale = glm::vec3(1.0f,1.0f,1.0f);
	};

	struct MeshRenderer {
		GLuint meshID;
	};



	struct DirectionalLight {
		glm::vec3 direction;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};
}


