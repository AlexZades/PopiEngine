#include "ui.h"
#include "utils.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


using namespace PopiEngine::Logging;
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
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(_window, true);
		ImGui_ImplOpenGL3_Init("#version 450");
	}	

	//Call one per fram to update ImGui state
	void UICore::NewFrame() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

}

