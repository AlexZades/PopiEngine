#include "ui.h"
#include "utils.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <entity.h>
#include <graphics.h>
#include <components.h>
#include <glm/gtc/type_ptr.hpp>

using namespace PopiEngine::Logging;
using namespace PopiEngine::ECS;
using namespace PopiEngine::Graphics;
namespace PopiEngine::UI
{

	UICore::UICore(GLFWwindow* window) {
		LogNormal("Initializing Ui Core");
		_window = window;
		InitializeImGui();
	}
	UICore::~UICore() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void UICore::InitializeImGui() {
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(_window, true);
		ImGui_ImplOpenGL3_Init("#version 450");
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui::StyleColorsDark();
	}	

	//Call one per fram to update ImGui state
	void UICore::NewFrame() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}


	void UICore::UpdateEntites() {
		if(entities.size() -1 < selectedEntityIndex) {
			selectedEntityIndex = 0; // Reset to 0 if the index is out of bounds
		}
	}

	void UICore::DrawEditor(GLuint viewTexture) {
		 // Create a dockspace that covers the entire viewport
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		
		ImGui::Begin("DockSpace Demo", nullptr, window_flags);
		ImGui::PopStyleVar(3);
		
		// Create the dockspace
		ImGuiID dockspace_id = ImGui::GetID("Dock");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		
		ImGui::End();
		
		DrawHeirarchy();
		DrawSceneView(viewTexture);
		DrawInspector();
	}
	
	void UICore::DrawSceneView(GLuint viewTexture) {
		ImGui::Begin("Scene View");
		{
			ImGui::BeginChild("Render");

			// Get the current available content region size
			ImVec2 viewportSize = ImGui::GetContentRegionAvail();

			// Check if viewport size has changed and update if necessary
			static float lastWidth = 0;
			static float lastHeight = 0;
			if (viewportSize.x != lastWidth || viewportSize.y != lastHeight) {
				if (viewportSize.x > 0 && viewportSize.y > 0) {
					// Resize the editor framebuffer to match the ImGui window size
					if (PopiEngine::Graphics::activeGraphicsCore) {
						PopiEngine::Graphics::activeGraphicsCore->ResizeEditorViewport(viewportSize.x, viewportSize.y);
					}
					lastWidth = viewportSize.x;
					lastHeight = viewportSize.y;
				}
			}

			// Only render if we have valid dimensions
			if (viewportSize.x > 0 && viewportSize.y > 0) {
				ImGui::Image(
					(ImTextureID)viewTexture,
					viewportSize,
					ImVec2(0, 1),
					ImVec2(1, 0)
				);
			}

			ImGui::EndChild();
		}
		ImGui::End();
	}
	void UICore::DrawHeirarchy() {
		UpdateEntites();
		ImGui::Begin("Heirachy", &editorTools, ImGuiWindowFlags_MenuBar);

		ImGui::BeginChild("Entites");
		ImGui::BeginListBox("##listbox"); {
			for (int i = 0; i < entities.size(); i++) {
				const auto& entity = entities[i];
				const bool is_selected = (selectedEntityIndex == i);
				if (entity->isActive) {
					if (ImGui::Selectable(format("[{}]{}", i, entity->name).c_str(), is_selected)){
						selectedEntityIndex = i; // Update the selected entity index
					}
				}
			}
		}
		ImGui::EndListBox();
		ImGui::EndChild();
		ImGui::End();
	}

	void UICore::DrawInspector() {
		//Safety check
		if (selectedEntityIndex < 0 || selectedEntityIndex >= entities.size()) {
			return; 
		}
		auto& entity = entities[selectedEntityIndex];
		ActiveComponents activeComponents = entity->GetActiveComponents();
		ImGui::Begin("Inspector");
		if(activeComponents & TRANSFORM) {
			TransformGizmo(entity->transform);
		}

		ImGui::End();
	}

#pragma region Component Spesific UI
	void UICore::TransformGizmo(std::shared_ptr<Transform> transform)
	{
		//To modify the transform of the selected entity we need to unpack
		//the glm::vec3 to a floats so imgui can modify them
		ImGui::SeparatorText("Transform");
		ImGui::InputFloat3("Position", glm::value_ptr(transform->position));
		ImGui::InputFloat3("Rotation", glm::value_ptr(transform->rotation));
		ImGui::InputFloat3("Scale", glm::value_ptr(transform->scale));
	}
#pragma endregion


}

