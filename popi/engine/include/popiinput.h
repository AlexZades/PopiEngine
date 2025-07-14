#pragma once



#include <components.h>
#include <utils.h>
#include <entity.h>
#include <GLFW/glfw3.h>
using namespace PopiEngine::ECS;
using namespace PopiEngine::Logging;
using std::string, std::format, std::vector;

namespace PopiEngine::Input
{
	class InputCore {
		public:
			
			InputCore(GLFWwindow* window);
			~InputCore();
			void ProcessInput(GLFWwindow* window);
	};
	
	
}