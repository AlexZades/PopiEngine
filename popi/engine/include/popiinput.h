#pragma once



#include <components.h>
#include <utils.h>
#include <entity.h>
#include <GLFW/glfw3.h>
#include <set>
using namespace PopiEngine::ECS;
using namespace PopiEngine::Logging;
using std::string, std::format, std::vector;

namespace PopiEngine::Input
{
	
	class InputCore {
		public:
			std::set<int> keysPressed; 
			std::set<int> mousePressed;
			double mouseX = 0.0, mouseY = 0.0;
			InputCore();

			~InputCore();

			void LinkCallbacks(GLFWwindow* window);

			//GLFW Callbacks
			
			
	};
	extern std::shared_ptr<InputCore> inputCoreRef;
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
	
	
}