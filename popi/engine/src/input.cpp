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
	std::shared_ptr<InputCore> inputCoreRef = std::make_shared<InputCore>();

	InputCore::InputCore()
	{
		
	}
	InputCore::~InputCore()
	{
	}
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		LogNormal(format("Scroll: xoffset: {}, yoffset: {}", xoffset, yoffset));
	}

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS) {
			inputCoreRef->keysPressed.insert(key);
		}
		else if (action == GLFW_RELEASE) {
			inputCoreRef->keysPressed.erase(key);
		}
	}

	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		if (action == GLFW_PRESS) {
			inputCoreRef->mousePressed.insert(button);
		}
		else if (action == GLFW_RELEASE) {
			inputCoreRef->mousePressed.erase(button);
		}
	}

	void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
		inputCoreRef->mouseX = xpos;
		inputCoreRef->mouseY = ypos;
	}

	void InputCore::LinkCallbacks(GLFWwindow* window) {
		LogNormal("Linking input callbacks to window");
		glfwSetScrollCallback(window, ScrollCallback);
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetMouseButtonCallback(window, MouseButtonCallback);
		glfwSetCursorPosCallback(window, CursorPositionCallback);
		LogNormal("Input callbacks linked");
	}

}