#pragma once  
#include <string>  

static const char* RESOURCES_SHADERS = "./resources/shaders";
static const char* RESOURCES = "./resources";
static const char* RESOURCES_TEXTURES = "./resources/textures";
static const char* RESOURCES_MESHES = "./resources/mesh";

//Name of default models
static const char* PRIMATIVE_CUBE = "cube";
static const char* PRIMATIVE_UVCUBE = "UVCube";
static const char* PRIMATIVE_CYL = "cyl";
static const char* PRIMATIVE_PLANE = "plane";
static const char* PRIMATIVE_PYRAMID = "pyramid";
namespace PopiEngine  
{  
	/// <summary>  
	/// Settings struct for PopiEngine.  
	/// </summary>  
	struct Settings  
	{  
		
		bool editorMode = true; 
		int windowWidth = 1000;  // Default width of the window  
		int windowHeight = 1000; // Default height of the window  
		const std::string windowName = "PopiEngine"; // Default name of the window  

		const std::string unlitShader = "unlit";
		const std::string litShader = "lit";
	};  
}