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
	}
	InputCore::~InputCore()
	{
	}
	void InputCore::ProcessInput(GLFWwindow* window) {

	}

}