#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <components.h>

namespace PopiEngine::Graphics
{
	class Camera
	{
	public:
		glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 defaultUp = glm::vec3(0.0f, 1.0f, 0.0f);
		float yaw; 
		float pitch; 

		float speed;
		float sensitivity;

		Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float speed, float sensitivity);

		glm::mat4 GetViewMatrix();

	private:
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 side = glm::vec3(0.0f, 0.0f, 1.0f);
		void RecomputeCameraVectors();

	};
}