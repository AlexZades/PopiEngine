
#include <chrono>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <importer.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <string>	
#include <utils.h>
#include "graphics.h"
#include "importer.h"
#include "settings.h"
#include "ui.h"

using std::string, std::ifstream, std::stringstream,std::format;

using namespace PopiEngine::Importer;
using namespace PopiEngine::Logging;
using namespace PopiEngine::UI;
namespace PopiEngine::Graphics 
{
#pragma region Graphics Core
    GraphicsCore::GraphicsCore() {
        LogNormal("Initializing Graphics Core");

        InitializeGL();
    }
    GraphicsCore::~GraphicsCore() {
        glfwDestroyWindow(window);

        glfwTerminate();
    }
    void GraphicsCore::SetUiCore(UICore* _uiCore) {
		uiCore = _uiCore;
	}

    void GraphicsCore::InitializeGL() {
        LogNormal("Initializing OpenGL...");
        if (!glfwInit())
            LogError("Error initializing glfw");
        Settings settings;
        window = InitializeWindow(settings.windowWidth, settings.windowHeight, settings.windowName);

        if (!gladLoaderLoadGL())
            LogError("Error initializing glad");

        glfwSwapInterval(1);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(message_callback, nullptr);

    }


    /// <summary>
    /// Initializes a GLFW window with H height, V width, and name.
    /// </summary>
    /// <param name="H">Window width</param>
    /// <param name="V">Window Height</param>
    /// <param name="windowName">Window Name</param>
    /// <returns>Returns GLFW Window object or null on failure</returns>
    GLFWwindow* GraphicsCore::InitializeWindow(int H, int V, string windowName) {
        LogNormal("Initializing OpenGL Window");
        if (!glfwInit())
            return nullptr;

        GLFWwindow* window = glfwCreateWindow(H, V, windowName.data(), nullptr, nullptr);
        if (!window)
        {  //Close GLFW and return null if window creation fails
            glfwTerminate();
            return nullptr;
        }

        // Make the window's context current  
        glfwMakeContextCurrent(window);

        return window; // Return the created window  
    }

    GLFWwindow* GraphicsCore::GetWindow() {
        return window;
    }
    void GraphicsCore::Clear() {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void GraphicsCore::FrameStart(){
        glfwPollEvents();
       
	}
    void GraphicsCore::Draw() {
		//Code to draw the scene goes here
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
		Clear();
    }
#pragma endregion

        



     map<string, std::shared_ptr<ShaderProgram>> shaderPrograms;

#pragma region Shaders

     ShaderProgram::ShaderProgram(string shaderName) {
         LogNormal(std::format("Creating shader program {}", shaderName));
         name = shaderName;
         programID = glCreateProgram();


         //Find and attach shaders    
         if (shaderPaths.find(name) == shaderPaths.end()) {
             LogError(std::format("Shader {} not found!", shaderName));
         }
         for (auto shaderPathObject : shaderPaths[name]) {
             Attach(shaderPathObject.path, shaderPathObject.type);
         }
         Link();

     }

     shared_ptr<ShaderProgram> InitalizeShader(string shaderName) {
         if (shaderPrograms.find(shaderName) != shaderPrograms.end()) {
             LogWarning(std::format("Shader {} already initialized!", shaderName));
             return nullptr;
         }
         auto shaderProgram = std::make_shared<ShaderProgram>(shaderName);
         shaderPrograms[shaderName] = shaderProgram;
         return shaderProgram;
     }

     ShaderProgram::~ShaderProgram() {
         //Detach and delete shaders
         for (auto shaderID : attachedShaderIDs) {
             if (shaderID != 0) {
                 glDetachShader(programID, shaderID);
                 glDeleteShader(shaderID);
             }
         }
         glDeleteProgram(programID);
         LogNormal(std::format("Deleted shader program: {}", programID));
     }
     void ShaderProgram::Attach(string filepath, GLenum shaderType) {
         GLuint shaderID = glCreateShader(shaderType);
         string shaderCode = LoadSource(filepath);


         if (shaderCode.empty()) {
             LogError(std::format("Shader is empty?: {}", filepath));
             return;
         }
         const char* sourceCStr = shaderCode.c_str();
         glShaderSource(shaderID, 1, &sourceCStr, NULL);
         glCompileShader(shaderID);

         // Check for shader compile errors
         GLint success;
         glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
         if (!success) {
             LogError(format("Error compiling shader id: {}\n", shaderID));
             return;
         }

         glAttachShader(programID, shaderID);

         switch (shaderType) {
         case GL_VERTEX_SHADER:
             attachedShaderIDs[0] = shaderID;
             break;
         case GL_FRAGMENT_SHADER:
             attachedShaderIDs[1] = shaderID;
             break;
         default:
             LogError(std::format("Shader Type Not Implemented: {}", shaderType));
             return;
         }
         LogNormal(std::format("Attached shader: {} to program: {}", filepath, programID));

     }

     struct ShaderCode {
         string source;
         GLenum type;
     };

     GLuint ShaderProgram::GetId() {
         return programID;
     }

     void ShaderProgram::Use() {
         glUseProgram(programID);
     }
     void ShaderProgram::Link() {
         glLinkProgram(programID);
         // Check for linking errors
         GLint success;
         glGetProgramiv(programID, GL_LINK_STATUS, &success);
         if (!success) {
             char infoLog[1024];
             glGetProgramInfoLog(programID, 1024, NULL, infoLog);

             LogError(std::format("Error linking shader program: {}\n{}", programID, infoLog));
         }
         else {
             LogNormal(std::format("Linked shader program: {}", programID));
         }
     }

     string ShaderProgram::LoadSource(const string& filepath) {
         ifstream reader;
         string source;

         stringstream stream;
         //Open the shader file
         reader.open(filepath, std::ios::in);
         if (reader.fail()) {
             LogError(std::format("Failed to open shader file: {}", filepath));
             return "";
         }
         //Read the file contents into the string
         stream << reader.rdbuf();
         reader.close();
         source = stream.str();

         return source;

     }
#pragma endregion


    
}
