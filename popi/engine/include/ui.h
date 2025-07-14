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
		void DrawMenuBar();
		void UpdateEntites();

		//Menu Bar Menus
		void FileMenu();
		void EditMenu();
		void InspectorMenu();
		void AddComponentMenu();
		int selectedMesh = 0;

		void AddMeshRendererMenu();
		//Resources Menu
		void ResourcesMenu();
		void TextureMenu();
		void ShaderMenu();
		void MeshMenu();

		//Resource Preview menu
		void ResourcePreviewMenu();
		//Inspector gizmo draw calls
		void TransformGizmo(std::shared_ptr<Transform> transform);
		void MeshRendererGizmo(std::shared_ptr<MeshRenderer> meshRenderer);
		void MeshGizmoAddTexture(std::shared_ptr<MeshRenderer> meshRenderer);
		void CameraGizmo(std::shared_ptr<ECS::Camera> camera);
		void PontLightGizmo(std::shared_ptr<PointLight> pointLight);
		void DirectionalLightGizmo(std::shared_ptr<DirectionalLight> directionalLight);
	};

	struct UIElement {
		
	};


}
