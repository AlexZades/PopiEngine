#pragma once


#include <components.h>
#include <utils.h>
#include <entity.h>
#include <popiinput.h>
#include <GLFW/glfw3.h>
using namespace PopiEngine::ECS;
using namespace PopiEngine::Logging;
using std::string, std::format, std::vector;
namespace PopiEngine::Input
{
	InputCore::InputCore(GLFWwindow* window)
	{
		//glfwSetCursorPosCallback(window, CursorPositionCallback);
	}
	InputCore::~InputCore()
	{
	}
	void InputCore::ProcessInput(GLFWwindow* window) {

	}

	void InputCore::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		LogNormal(format("Scroll: xoffset: {}, yoffset: {}", xoffset, yoffset));
	}

	void InputCore::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS) {
			LogNormal(format("Key Pressed: {}, Scancode: {}, Mods: {}", key, scancode, mods));
		}
		else if (action == GLFW_RELEASE) {
			LogNormal(format("Key Released: {}, Scancode: {}, Mods: {}", key, scancode, mods));
		}
	}

	void InputCore::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		if (action == GLFW_PRESS) {
			LogNormal(format("Mouse Button Pressed: {}, Mods: {}", button, mods));
		}
		else if (action == GLFW_RELEASE) {
			LogNormal(format("Mouse Button Released: {}, Mods: {}", button, mods));
		}
	}

	void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
		LogNormal(format("Mouse Position: X: {}, Y: {}", xpos, ypos));
	}

}