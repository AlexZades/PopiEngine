#pragma once  
#include <string>  

static const char* RESOURCES_SHADERS = "./resources/shaders";
static const char* RESOURCES = "./resources";
static const char* RESOURCES_TEXTURES = "./resources/textures";
namespace PopiEngine  
{  
	/// <summary>  
	/// Settings struct for PopiEngine.  
	/// </summary>  
	struct Settings  
	{  
		int windowWidth = 1000;  // Default width of the window  
		int windowHeight = 1000; // Default height of the window  
		const std::string windowName = "PopiEngine"; // Default name of the window  

		const std::string unlitShader = "unlit";
		const std::string litShader = "lit";
	};  
}