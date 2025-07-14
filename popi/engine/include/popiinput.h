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
			vector<int> pressedKeys;
			InputCore(GLFWwindow* window);
			~InputCore();

			void ProcessInput(GLFWwindow* window);

			//GLFW Callbacks
			void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
			void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
			void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
			void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
			
	};
	
	
}