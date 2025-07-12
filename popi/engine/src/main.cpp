#define GLFW_INCLUDE_NONE

#include <chrono>
#include <glad/gl.h> //always include glad before glfw
#include <glm/glm.hpp>
#include <GLFW/glfw3.h> 
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <thread>
#include <graphics.h>
#include <settings.h>
#include <ui.h>
#include <project.h>
#include <utils.h>
using namespace PopiEngine::Graphics;
using namespace PopiEngine;
using namespace PopiEngine::UI;
using namespace PopiEngine::Logging;
using std::string, std::runtime_error;

 // Initialize settings
string popiLogo = R"(
┏━━━┓━━━━━━━━━━━━━━━━━━┏━━━┓━━━━━━━━━━━━━━━━━━
┃┏━┓┃━━━━━━━━━━━━━━━━━━┃┏━━┛━━━━━━━━━━━━━━━━━━
┃┗━┛┃┏━━┓┏━━┓┏━━┓┏┓━━━━┃┗━━┓┏━┓━┏━━┓┏┓┏━┓━┏━━┓
┃┏━━┛┃┏┓┃┃┏┓┃┃┏┓┃┣┫━━━━┃┏━━┛┃┏┓┓┃┏┓┃┣┫┃┏┓┓┃┏┓┃
┃┃━━━┃┗┛┃┃┗┛┃┃┗┛┃┃┃━━━━┃┗━━┓┃┃┃┃┃┗┛┃┃┃┃┃┃┃┃┃━┫
┗┛━━━┗━━┛┃┏━┛┃┏━┛┗┛━━━━┗━━━┛┗┛┗┛┗━┓┃┗┛┗┛┗┛┗━━┛
━━━━━━━━━┃┃━━┃┃━━━━━━━━━━━━━━━━━┏━┛┃━━━━━━━━━━
━━━━━━━━━┗┛━━┗┛━━━━━━━━━━━━━━━━━┗━━┛━━━━━━━━━━
)";

int main(int argc, char* argv[]) {
	LogNormal(popiLogo);
	try {

		Project project;
		return project.Run();
	}
	catch (const std::exception& e) {
		LogError(e.what());
		return EXIT_FAILURE;
	}
}