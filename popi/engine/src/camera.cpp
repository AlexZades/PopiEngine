#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <camera.h>
#include <utils.h>

using namespace PopiEngine::Logging;

namespace PopiEngine::Graphics {


	Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float speed, float sensitivity) {
		this->Position = position, this->defaultUp = up, this->yaw = yaw, this->pitch = pitch, this->speed = speed, this->sensitivity = sensitivity;
		LogNormal("Initializing Camera");
		RecomputeCameraVectors();
	}

	glm::mat4 Camera::GetViewMatrix() {
		return glm::lookAt(Position, Position + Front, defaultUp);
	}

	void Camera::RecomputeCameraVectors() {
		//Compute the new vectors based on the current yaw and pitch
		glm::vec3 newFront;
		
		newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		newFront.y = sin(glm::radians(pitch));
		newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		Front = glm::normalize(newFront);
		side = glm::normalize(glm::cross(Front,defaultUp));
		up = glm::normalize(glm::cross(side, Front));
	}
}
