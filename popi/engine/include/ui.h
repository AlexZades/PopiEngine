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
#include <entity.h>
#include <components.h>
using std::vector, std::shared_ptr, std::string, std::format;
using namespace PopiEngine::ECS;
namespace PopiEngine::UI
{
	class UICore {
	public:
		UICore(GLFWwindow* window);
		~UICore();
		void InitializeImGui();
		int selectedEntityIndex = 0;
		void NewFrame();
		void DrawEditor(GLuint viewTexture);

		GLFWwindow* _window;
		bool editorTools = true;
	private:
		void DrawSceneView(GLuint viewTexture);
		void DrawHeirarchy();
		void DrawInspector();
		void UpdateEntites();
		//Inspector gizmo draw calls
		void TransformGizmo(std::shared_ptr<Transform> transform);
	};

	struct UIElement {
		
	};


}
