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
#include <ui.h>
using std::string, std::vector, std::map, std::shared_ptr;
using namespace PopiEngine::UI;	
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

		//Utils
		
		//Basic Setters
		void setBool(const string& varName, bool value);
		void setInt(const string& varName, int value);
		void setFloat(const string& varName, float value);

		//Vector Setters
		void setVec2(const string& varName, const glm::vec2& value);
		void setVec3(const string& varName, const glm::vec3& value);
		void setVec4(const string& varName, const glm::vec4& value);

		//Material Setters
		void setMat2(const string& varName, const glm::mat2& value);
		void setMat3(const string& varName, const glm::mat3& value);
		void setMat4(const string& varName, const glm::mat4& value);

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

		void SetUiCore(UICore* _uiCore);
		void Clear();
		void Draw();
		void FrameStart();

	private:
		GLFWwindow* window = nullptr;
		UICore* uiCore = nullptr;
	};

	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	struct Texture {
		GLuint id;
		string type;
		string path;
	};

	class Mesh {
	public:
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<Texture> textures;

		std::shared_ptr<ShaderProgram> shaderProgram;
		GLuint VAO;
		
		Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures,string shaderProgramName);
		void Draw();

	private:
		GLuint  VBO, EBO;

		void InitalizeMesh();
	};

	struct Material {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
	};	
}