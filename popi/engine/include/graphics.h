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
#include <camera.h>
#include <settings.h>
using std::string, std::vector, std::map, std::shared_ptr;
using namespace PopiEngine::UI;	
/// <summary>
/// Graphic utilities for PopiEngine
/// 
/// GraphicsCore manages the OpenGL context, window, and rendering loop.
/// 
/// Other graphics related classes are also here.
/// </summary>
namespace PopiEngine::Graphics
{

	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	enum TextureType {
		DIFFUSE,
		SPECULAR,
		NORMAL,
		HEIGHT
	};

	struct Texture {
		GLuint id;
		TextureType type;
		string path;

		Texture(string _path, TextureType _type);
		
	};



	//General Material Struct
	//Currently not using it
	struct Material {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
	};

	/// <summary>
	/// A ShaderProgram Class
	/// </summary>
	class ShaderProgram {
	public:
		ShaderProgram(string shaderName);
		~ShaderProgram();
		string name;
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

		
		GLuint programID = 0;
		GLuint attachedShaderIDs[2]= {};
	};

	class Mesh {
	public:
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<Texture> textures;

		std::shared_ptr<ShaderProgram> shaderProgram;
		GLuint VAO;

		Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, string shaderProgramName);
		void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 proj);

	private:
		GLuint  VBO, EBO;

		void InitalizeMesh();
		void GetTextures();
	};

	shared_ptr<ShaderProgram> InitalizeShader(string shaderName);
	/// <summary>
	/// Vertex = 0, Fragment = 1... Add more as needed
	/// </summary>
	extern map<string, shared_ptr<ShaderProgram>> shaderPrograms;

	class GraphicsCore {

	public:
		GraphicsCore(bool enableEditorMode = false);
		~GraphicsCore();

		map<GLuint, std::shared_ptr<Mesh>> activeMeshes;
		std::shared_ptr<Entity> activeCamera;


		Settings settings = Settings();

		string windowName = "";
		int windowWidth = 0;
		int windowHeight = 0;
		bool editorMode;

		void InitializeGL();
		void SetUiCore(UICore* _uiCore);
		void Clear();
		void Draw();

		std::shared_ptr<Entity> GetActiveCamera();

		

		void FrameStart();
		GLFWwindow* InitializeWindow(int H, int V, string windowName);
		GLFWwindow* GetWindow();
		GLuint LinkMesh(std::shared_ptr<Mesh> mesh);
		void LinkCamera(std::shared_ptr<Entity> camera);

		//Editor specific methods
		GLuint GetEditorTexture();
		void ResizeEditorViewport(float width, float height);

		float editorViewportWidth = 400.0f;
		float editorViewportHeight = 400.0f;
	private:
		GLuint editorFBO = 0;
		GLuint editorRBO = 0;
		GLuint editorTexture = 0;


		void RenderEntities(glm::mat4 proj, glm::mat4 view);

		glm::mat4 CalulateViewMatrix(std::shared_ptr<Entity> cameraEntity);
		glm::mat4 GetProjectionMatrix(std::shared_ptr<Entity> cameraEntity, float aspectRatio);

		//These are used for the edtiror
		void InitalizeFrameBuffer();
		
		GLFWwindow* window = nullptr;
		UICore* uiCore = nullptr;
	};

	void ResizeCallback(GLFWwindow* window, int width, int height);
	extern GraphicsCore* activeGraphicsCore; //Thhis is terrible please fix it later

	//Primative Mesh Creation Functions
	std::shared_ptr<Mesh> CreateCube(string shaderProgramName);
	std::shared_ptr<Mesh> CreateCube(string shaderProgramName, vector<Texture> textures);
}