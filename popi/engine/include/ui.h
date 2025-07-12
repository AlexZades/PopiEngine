#pragma once
#include <chrono>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <thread>
#include <string>	
#include <vector>

using std::vector, std::shared_ptr, std::string, std::format;

namespace PopiEngine::UI
{
	class UICore {
	public:
		UICore(GLFWwindow* window);
		~UICore();
		void InitializeImGui();

		void NewFrame();

		GLFWwindow* _window;
	};

	struct UIElement {
		
	};

}
