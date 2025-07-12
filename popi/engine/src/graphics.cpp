#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include <chrono>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

using std::string, std::ifstream, std::stringstream, std::format;

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
        glEnable(GL_DEPTH_TEST);
        glDebugMessageCallback(message_callback, nullptr);

        activeMeshes = map<GLuint, std::shared_ptr<Mesh>>();

    }

    GLuint GraphicsCore::LinkMesh(std::shared_ptr<Mesh> mesh) {
        if (mesh == nullptr) {
            LogError("Mesh is null, cannot link");
            return 0;
        }
        GLuint id = mesh->VAO;
        activeMeshes[id] = mesh;
        return id;
	}

    void GraphicsCore::LinkCamera(std::shared_ptr<Camera> camera) {
        if (camera == nullptr) {
            LogError("Camera seems not to be set, null ptr");
            return;
        }
        this->activeCamera = camera;
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
        glClearColor(0.53f, 0.81f, 0.92f, 1.0f); // Use float literals to avoid warnings
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void GraphicsCore::FrameStart(){
        glfwPollEvents();
       
	}

    void GraphicsCore::Draw() {
		//Code to draw the scene goes here
        Clear(); // Clear before drawing, not after
 
        glm::mat4 projection = glm::perspective(
            glm::radians(70.0f), // Field of view in radians
            800.0f / 800.0f,     // Aspect ratio
            0.1f,                // Near clipping plane
            100.0f               // Far clipping plane
        );
        
        glm::mat4 view = activeCamera->GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f); // Identity matrix for model transformation
        
        for (const auto& item : activeMeshes) {
            auto& mesh = item.second;
            // Use the mesh's shader program and set matrices
            mesh->shaderProgram->Use();
            mesh->shaderProgram->setMat4("projection", projection);
            mesh->shaderProgram->setMat4("view", view);
            mesh->shaderProgram->setMat4("model", model);
            mesh->Draw();
        }
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
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

     void ShaderProgram::setBool(const string& varName, bool value)
     {
		 glUniform1i(glGetUniformLocation(programID, varName.c_str()), (int)value);
     }
     void ShaderProgram::setInt(const string& varName, int value) 
     {
		 glUniform1i(glGetUniformLocation(programID, varName.c_str()), value);
     }
     void ShaderProgram::setFloat(const string& varName, float value)
     {
		 glUniform1f(glGetUniformLocation(programID, varName.c_str()), value);
     }

     //Vector Setters
     void ShaderProgram::setVec2(const string& varName, const glm::vec2& value)
     {
		 glUniform2f(glGetUniformLocation(programID, varName.c_str()), value.x, value.y);
     }
     void ShaderProgram::setVec3(const string& varName, const glm::vec3& value)
     {
		 glUniform3f(glGetUniformLocation(programID, varName.c_str()), value.x, value.y, value.z);
     }
     void ShaderProgram::setVec4(const string& varName, const glm::vec4& value)
     {
		 glUniform4f(glGetUniformLocation(programID, varName.c_str()), value.x, value.y, value.z, value.w);
     }

     //Material Setters
     void ShaderProgram::setMat2(const string& varName, const glm::mat2& value)
     {
		 glUniformMatrix2fv(glGetUniformLocation(programID, varName.c_str()), 1, GL_FALSE, &value[0][0]);
     }
     void ShaderProgram::setMat3(const string& varName, const glm::mat3& value)
     {
		 glUniformMatrix3fv(glGetUniformLocation(programID, varName.c_str()), 1, GL_FALSE, &value[0][0]);
     }
     void ShaderProgram::setMat4(const string& varName, const glm::mat4& value)
     {
		 glUniformMatrix4fv(glGetUniformLocation(programID, varName.c_str()), 1, GL_FALSE, &value[0][0]);
     }

#pragma endregion


#pragma region Mesh

     Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, string shaderProgramName) {
         this->vertices = vertices;
		 this->indices = indices;
         this->textures = textures;

         if(shaderPrograms.find(shaderProgramName) == shaderPrograms.end()) {
			 LogError(format("Invalid Shader Program: {}, has the shader been initalized?", shaderProgramName));
             return;
		 }
         shaderProgram = shaderPrograms[shaderProgramName];


		 InitalizeMesh();
     }

     void Mesh::InitalizeMesh() {
		 glGenVertexArrays(1, &VAO);

		 glGenBuffers(1, &VBO);
		 glGenBuffers(1, &EBO);

		 glBindVertexArray(VAO);
         // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		 glBindBuffer(GL_ARRAY_BUFFER, VBO);

         glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
     
		 glEnableVertexAttribArray(0);
         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

		 glEnableVertexAttribArray(1);
		 glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

         glEnableVertexAttribArray(2);
         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
     
		 glBindVertexArray(0); // Unbind VAO
     }

     void Mesh::Draw() {
		 GetTextures();

		 //Bind VAO and draw mesh
		 glBindVertexArray(VAO);
         glDrawElements(GL_TRIANGLES, static_cast<GLuint>(indices.size()), GL_UNSIGNED_INT, 0);
		 

		 //Unbind vao and cleanup 
         glBindVertexArray(0); 
		 glActiveTexture(GL_TEXTURE0); 

	 }

     void Mesh::GetTextures() {
         GLuint diffuse = 1, specular = 1, normal = 1, height = 1;
         for (GLuint i = 0; i < textures.size(); i++) {
             glActiveTexture(GL_TEXTURE0 + i);

             string number;
             string name;
             TextureType type = textures[i].type;

             switch (type) {
             case DIFFUSE:
                 number = std::to_string(diffuse++);
				 name = "material.DIFFUSE";
                 break;
             case SPECULAR:
                 number = std::to_string(specular++);
                 name = "material.SPECULAR";
                 break;
             case NORMAL:
                 number = std::to_string(normal++);
                 name = "material.NORMAL";
                 break;
             case HEIGHT:
                 number = std::to_string(height++);
                 name = "material.HEIGHT";
                 break;
             default:
                 continue;

             }
             
             glUniform1i(glGetUniformLocation(shaderProgram->GetId(), (name + number).c_str()), i);
			 glBindTexture(GL_TEXTURE_2D, textures[i].id);
         }
     }
#pragma endregion

#pragma region Textures
Texture::Texture(string name, TextureType type) {
    if(texturePaths.find(name) == texturePaths.end()) {
        LogError(std::format("Texture {} not found!", name));
        return;
	}
    this->path = texturePaths[name].path;
    this->type = type;
	id = 0;

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data == nullptr) {
        LogError(std::format("Failed to load texture: {}", path));
        return;
    }
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
	LogNormal(std::format("Loaded texture: {} with id: {}", path, id));
    stbi_image_free(data);
}
#pragma endregion
// Add this function to create a default cube
std::shared_ptr<Mesh> CreateCube(string shaderProgramName) {
    // Cube vertices with Position, Normal, and Texture Coordinates
    vector<Vertex> vertices = {
        // Front face
        {{-0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},

        // Back face
        {{-0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},

        // Left face
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f,  0.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f,  0.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f,  0.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f,  0.0f}, {0.0f, 1.0f}},

        // Right face
        {{ 0.5f, -0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},

        // Top face
        {{-0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},

        // Bottom face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}
    };

    // Cube indices (two triangles per face)
    vector<GLuint> indices = {
        // Front face
        0, 1, 2,   2, 3, 0,
        // Back face
        4, 5, 6,   6, 7, 4,
        // Left face
        8, 9, 10,  10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Top face
        16, 17, 18, 18, 19, 16,
        // Bottom face
        20, 21, 22, 22, 23, 20
    };

	vector<Texture> textures; // No textures for the default cube
    // Create and return the mesh
    return std::make_shared<Mesh>(vertices, indices, textures, shaderProgramName);
}

}
