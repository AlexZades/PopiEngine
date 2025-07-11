#include "graphics.h"
#include "importer.h"
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

using std::string, std::ifstream, std::stringstream;

using namespace PopiEngine::Importer;
namespace PopiEngine::Graphics {

     map<string, std::shared_ptr<Shader>> shaders;


    /// <summary>
    /// Initalize GLFW window with specified height, width, and name.
    /// </summary>
    /// <param name="H">Window width in pixels</param>
    /// <param name="V">Window Height in pixels</param>
    /// <param name="windowName"> Window name as a string</param>
    /// <returns>Returns pointer to the window if sucessful, otherwise returns null</returns>
    GLFWwindow* InitalizeWindow(int H, int V, string windowName)
    {
        // return null if GLFW is not initialized
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



    /// <summary>
	/// Shader class for loading and using shaders.
    /// </summary>
    Shader::Shader(GLenum shaderType, const string& filepath) {
        this->shaderType = shaderType;
        shaderCode = LoadSource(filepath);
        shaderID = CompileShader();
    }


        string Shader::LoadSource(string filepath) {
            ifstream reader;
            string source;

            stringstream stream;
            //Open the shader file
            reader.open(filepath, std::ios::in);
            if (reader.fail()) {
                std::cerr << std::format("Error opening shader file: {}\n", filepath);
                return "";
            }
            //Read the file contents into the string
            stream << reader.rdbuf();
            reader.close();
            source = stream.str();

            return source;

        }

        GLuint Shader::CompileShader() {

            GLuint shaderID = glCreateShader(shaderType);

            if (shaderID == 0) {
                std::cerr << "Failed to create shader!\n";
            }

            const char* sourceCStr = shaderCode.c_str();
            std::cout << std::format("Compiling shader program {}.\n",sourceCStr);

            // Set the source code for the shader
            glShaderSource(shaderID, 1, &sourceCStr, nullptr);
            glCompileShader(shaderID);

            // Check for compilation errors
            GLint success;
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
            if (!success) {
                std::cerr << std::format("Error compiling shader id: {}\n", shaderID);
            }

            return shaderID;
        }        


        ShaderProgram::ShaderProgram(const vector<std::shared_ptr<Shader>>& programShaders) {
            
            programID = glCreateProgram();
            if (programID == 0) {
                std::cerr << "Failed to create program!\n";
            }
            for (const auto& shader : programShaders) {
                if (shader && shader->shaderID != 0) {
                    glAttachShader(programID, shader->shaderID);
					std::cout << std::format("Attaching shader id: {} to program id: {}\n", shader->shaderID, programID);
                }

            }

            glLinkProgram(programID);

            GLint success;
            glGetProgramiv(programID, GL_LINK_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetProgramInfoLog(programID, 512, nullptr, infoLog);
                std::cerr << "Error linking shader program: " << infoLog << std::endl;
            }
            for (const auto& shader : programShaders) {
                if (shader && shader->shaderID != 0)
                    glDetachShader(programID, shader->shaderID);
            }
        }

        void ShaderProgram::Use() {
            glUseProgram(programID);
        }
    /// <summary>
	/// Call this to load all shaders from the resources/shaders directory.
    /// </summary>
    void InitalizeShaders() {
		shaders = ImportShaders();
    }
    
}
