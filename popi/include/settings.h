#pragma once  
#include <string>  

static const char* RESOURCES_SHADERS = "./resources/shaders";

namespace PopiEngine  
{  
	/// <summary>  
	/// Settings struct for PopiEngine.  
	/// </summary>  
	struct Settings  
	{  
		int windowWidth = 1000;  // Default width of the window  
		int windowHeight = 1000; // Default height of the window  
		std::string windowName = "PopiEngine"; // Default name of the window  

		std::string defaultVertexShader = "unlit.vert";
		std::string defaultFragmentShader = "unlit.frag";
	};  
}