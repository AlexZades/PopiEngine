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
#include <map>
using std::string, std::vector, std::map;

/// <summary>
/// Graphic utilities for PopiEngine.
/// </summary>
namespace PopiEngine::Graphics
{
	GLFWwindow* InitalizeWindow(int H, int V, string windowName);



	struct Shader {
		GLuint shaderID = 0;
		GLenum shaderType = 0;
		Shader() = default; // Default constructor
		Shader(GLenum shaderType, const string& filepath);

		string shaderCode;
		GLuint CompileShader();
		string LoadSource(string filepath);

	};

	class ShaderProgram {
	public:
		void Use();
		void Attach(vector<string> filePaths);


		GLuint programID= 0;

		ShaderProgram( vector<std::shared_ptr<Shader>> programShaders);
	private:
		void LinkProgram();
		void DetachShaders();
	};

	extern map<string, std::shared_ptr<Shader>> shaders;

	void InitalizeShaders();
}