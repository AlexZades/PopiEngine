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
using std::string, std::vector, std::map, std::shared_ptr;

/// <summary>
/// Graphic utilities for PopiEngine.
/// </summary>
namespace PopiEngine::Graphics
{




	/// <summary>
	/// A ShaderProgram Class
	/// </summary>
	class ShaderProgram {
	public:
		ShaderProgram(string shaderName);
		~ShaderProgram();

		GLuint GetId();

		void Use();
	private:
		void Attach(string filePath, GLenum shaderType);
		void Link();


		string LoadSource(const string& filepath);

		string name;
		GLuint programID = 0;
		GLuint attachedShaderIDs[2]= {};
	};
	shared_ptr<ShaderProgram> InitalizeShader(string shaderName);
	/// <summary>
	/// Vertex = 0, Fragment = 1... Add more as needed
	/// </summary>
	extern map<string, shared_ptr<ShaderProgram>> shaderPrograms;

	class GraphicsCore {

	public:
		GraphicsCore();
		~GraphicsCore();

		GLFWwindow* GetWindow();

		void InitializeGL();
		GLFWwindow* InitializeWindow(int H, int V, string windowName);

		void Clear();
		void Draw();

	private:
		GLFWwindow* window = nullptr;

	};

	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	struct Texture {
		GLuint id;
	};

	/*class Mesh {
	public:
		vector<Vertex>       vertices;
		vector<unsigned int> indices;
		vector<Texture>      textures;

		
		Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
		void Draw(Shader& shader);

	private:
		unsigned int VAO, VBO, EBO;

		void setupMesh();
	};

	struct Material {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
	};*/
	
}