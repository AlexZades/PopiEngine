#define STB_IMAGE_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL 

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
#include <entity.h>
#include <components.h>
#include <glm/gtx/euler_angles.hpp>
#include <importer.h>

using std::string, std::ifstream, std::stringstream, std::format;

using namespace PopiEngine::Importer;
using namespace PopiEngine::Logging;
using namespace PopiEngine::ECS;
using namespace PopiEngine::UI;
namespace PopiEngine::Graphics 
{
    // This should be singleton
    GraphicsCore* activeGraphicsCore = nullptr;
    
#pragma region Callbacks
    void ResizeCallback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        LogNormal(std::format("Resized window to {}x{}", width, height));

        activeGraphicsCore->windowWidth = width;
        activeGraphicsCore->windowHeight = height;
    }

    void ResizeFramebuffer(GLFWwindow* window, int width, int height) {
        // This function is called when the framebuffer size changes
        glViewport(0, 0, width, height);
        LogNormal(std::format("Resized framebuffer to {}x{}", width, height));
        activeGraphicsCore->windowWidth = width;
        activeGraphicsCore->windowHeight = height;
	}
#pragma endregion


#pragma region Graphics Core
    GraphicsCore::GraphicsCore(bool enableEditorMode) {
		editorMode = enableEditorMode;
        LogNormal("Initializing Graphics Core");
        activeGraphicsCore = this;
		windowHeight = settings.windowHeight;
		windowWidth = settings.windowWidth;
		windowName = settings.windowName;

        if(editorMode)
			windowName = "PopiEngine Editor";

        InitializeGL();

        if(editorMode)
			InitalizeFrameBuffer();
    }
    GraphicsCore::~GraphicsCore() {
        glfwDestroyWindow(window);

        glfwTerminate();
    }

    GLuint GraphicsCore::GetEditorTexture() {
        return editorTexture;
    }

    void GraphicsCore::SetUiCore(UICore* _uiCore) {
		uiCore = _uiCore;
	}

    void GraphicsCore::InitializeGL() {
        LogNormal("Initializing OpenGL...");
        if (!glfwInit())
            LogError("Error initializing glfw");
        
        window = InitializeWindow(windowWidth, windowHeight, windowName);
        glfwSetWindowUserPointer(window, this); // Set the user pointer to the current instance
		glfwSetFramebufferSizeCallback(window, ResizeCallback);
        if (!gladLoaderLoadGL())
            LogError("Error initializing glad");

        glfwSwapInterval(1);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDebugMessageCallback(message_callback, nullptr);

        activeMeshes = map<GLuint, std::shared_ptr<Mesh>>();

    }
    /// <summary>
	/// Links a mesh to the Graphics Core.
    /// we might not need this anymore 
    /// </summary>
    /// <param name="mesh"></param>
    /// <returns></returns>
    GLuint GraphicsCore::LinkMesh(std::shared_ptr<Mesh> mesh) {
        if (mesh == nullptr) {
            LogError("Mesh is null, cannot link");
            return 0;
        }
        GLuint id = mesh->VAO;
        activeMeshes[id] = mesh;
        return id;
	}
    /// <summary>
	/// Links camera to the Graphics Core.
    /// </summary>
    /// <param name="camera"></param>
    void GraphicsCore::LinkCamera(std::shared_ptr<Entity> camera) {
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
        if (editorMode) {
            glBindFramebuffer(GL_FRAMEBUFFER, editorFBO);
            glViewport(0, 0, (GLsizei)editorViewportWidth, (GLsizei)editorViewportHeight);
			Clear(); // Clear the framebuffer before drawing
        }
	}
    /// <summary>
	/// Renders all entites with a MeshRenderer component.
    /// </summary>
    void GraphicsCore::RenderEntities(glm::mat4 proj, glm::mat4 view) 
    {
        //To handle transparent entites put them in this array
		auto depthSortedEntities = std::map<float,std::shared_ptr<Entity>>();

        for (const auto& entityPtr : entities) {
            auto entity = entityPtr.get();
            //Check several conditions to see if we should render the entity
            if (entity != nullptr &&
                entity->isActive &&
                entity->GetActiveComponents() & ActiveComponents::MESH_RENDERER) {
				
                Transform* transform = nullptr;
                //If the as a transform component, we apply its transformation
                if (entity->GetActiveComponents() & ActiveComponents::TRANSFORM) {
                    transform = entity->transform.get();
                }
                else {
                    Transform* defaultTransform = new Transform();
                    transform = defaultTransform;
                }
                auto& meshRenderer = entity->meshRenderer;
                //Get the mesh based on the meshID in the MeshRenderer component
                
                if (meshRenderer && activeMeshes.find(meshRenderer->meshID) != activeMeshes.end()) {
                    if (meshRenderer->isTransparent) {
						float distance = glm::length(transform->position - GetActiveCamera()->transform->position);
						depthSortedEntities[distance] = entityPtr; // Sort by distance for transparency
                    }
                    else {
                        auto mesh = activeMeshes[meshRenderer->meshID];
                        if (mesh) {
                            glm::mat4 translation = glm::translate(glm::mat4(1.0f), transform->position);
                            glm::mat4 rotation = glm::eulerAngleYXZ(
                                glm::radians(transform->rotation.x),
                                glm::radians(transform->rotation.y),
                                glm::radians(transform->rotation.z)
                            );
                            glm::mat4 scale = glm::scale(glm::mat4(1.0f), transform->scale);
                            glm::mat4 model = translation * rotation * scale;
                            mesh->Draw(model, view, proj);
                        }
                    }
                }
            }

        }
         
		//Defered rendering for transparent entities
        for (const auto& [distance, entityPtr] : depthSortedEntities) {
            auto entity = entityPtr.get();

                Transform* transform = nullptr;
                if (entity->GetActiveComponents() & ActiveComponents::TRANSFORM) {
                    transform = entity->transform.get();
                }
                else {
                    Transform* defaultTransform = new Transform();
                    transform = defaultTransform;
                }
                auto& meshRenderer = entity->meshRenderer;
                if (meshRenderer && activeMeshes.find(meshRenderer->meshID) != activeMeshes.end()) {
                    auto mesh = activeMeshes[meshRenderer->meshID];
                    if (mesh) {
                        glm::mat4 translation = glm::translate(glm::mat4(1.0f), transform->position);
                        glm::mat4 rotation = glm::eulerAngleYXZ(
                            glm::radians(transform->rotation.x),
                            glm::radians(transform->rotation.y),
                            glm::radians(transform->rotation.z)
                        );
                        glm::mat4 scale = glm::scale(glm::mat4(1.0f), transform->scale);
                        glm::mat4 model = translation * rotation * scale;
                        mesh->Draw(model, view, proj);
                    }
                }
            
        }
     }  

    void GraphicsCore::Draw() {
		//Code to draw the scene goes here
        if (!editorMode) {
            Clear(); // Clear main window if not in editor mode
        }
 
        // Calculate aspect ratio based on current mode
        float aspectRatio;
        if (editorMode) {
            aspectRatio = (float)editorViewportWidth / (float)editorViewportHeight;
        } else {
            aspectRatio = (float)windowWidth / (float)windowHeight;
        }
        auto cam = GetActiveCamera();
		glm::mat4 projection = GetProjectionMatrix(cam, aspectRatio);
        
		
		glm::mat4 view = CalulateViewMatrix(cam);
        
		RenderEntities(projection, view); //The actual drawing is done in here


        if (editorMode) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, windowWidth, windowHeight);
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);

    }

    std::shared_ptr<Entity> GraphicsCore::GetActiveCamera() {
        if (activeCamera->camera == nullptr) {
            for(const auto& entity : entities) {
                if (entity.get()->GetActiveComponents() & ActiveComponents::CAMERA) {
                    activeCamera = entity;
                    LogNormal(std::format("Found active camera: {}", entity.get()->name));
                    break;
                }
			}
        }

        return activeCamera;
	}
    glm::mat4 GraphicsCore::GetProjectionMatrix(std::shared_ptr<Entity> cameraEntity,float aspectRatio) {
        auto camera = cameraEntity->camera;
		if (camera == nullptr) {    
            LogError("Active camera is null, cannot calculate view matrix");
            return glm::mat4(1.0f); // Return identity matrix if no active camera
        }
		glm::mat4 projection;
        switch(camera->mode) {
            case CameraMode::PERSPECTIVE:
				projection = glm::perspective(
                    glm::radians(camera->fov),
                    aspectRatio,         
                    camera->nearPlane,               
                    camera->farPlane
                );
				return projection;
            case CameraMode::ORTHOGRAPHIC:
               
                projection = glm::ortho(
                    -aspectRatio * camera->orthographicSize/2, 
                    aspectRatio * camera->orthographicSize / 2,
                    -camera->orthographicSize / 2,
                    camera->orthographicSize / 2,
                    camera->nearPlane, 
                    camera->farPlane
				);
				return projection;
            default:
                LogError("Inccorect camera mode");
                return glm::mat4(1.0f); 
		}
        return glm::mat4(1.0f);
	}
    glm::mat4 GraphicsCore::CalulateViewMatrix(std::shared_ptr<Entity> cameraEntity) {
		auto camera = cameraEntity->camera; 
		auto transform = cameraEntity->transform;
        if (camera == nullptr || transform == nullptr) {
            LogError("Camera or Transform component is null, cannot calculate view matrix");
            return glm::mat4(1.0f); // Return identity matrix if components are missing
        }
        
        // Convert Euler angles to direction vector
        // Assuming rotation.x = pitch, rotation.y = yaw, rotation.z = roll
        float pitch = glm::radians(transform->rotation.x);
        float yaw = glm::radians(transform->rotation.y);
        
        glm::vec3 front;
        front.x = cos(yaw) * cos(pitch);
        front.y = sin(pitch);
        front.z = sin(yaw) * cos(pitch);
        front = glm::normalize(front);

        return glm::lookAt(
            transform->position,           // Camera position
            transform->position + front,   // Look at point
            camera->defaultUp              // Up vector
        );
    }

    void GraphicsCore::InitalizeFrameBuffer() {
        editorViewportWidth = 400.0f;  // Set a default size
        editorViewportHeight = 400.0f;
        
        glGenFramebuffers(1, &editorFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, editorFBO);

		glGenTextures(1, &editorTexture);

        glBindTexture(GL_TEXTURE_2D, editorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)editorViewportWidth, (GLsizei)editorViewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, editorTexture, 0);

        glGenRenderbuffers(1, &editorRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, editorRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)editorViewportWidth, (GLsizei)editorViewportHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, editorRBO);
        
        // Check framebuffer completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            LogError("Editor framebuffer is not complete!");
        } else {
            LogNormal("Editor framebuffer initialized successfully");
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

    }

    void GraphicsCore::ResizeEditorViewport(float width, float height) {
        editorViewportWidth = width;
        editorViewportHeight = height;
        
		// Bind the framebuffer first before modifying its attachments
        glBindFramebuffer(GL_FRAMEBUFFER, editorFBO);
        
        glBindTexture(GL_TEXTURE_2D, editorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)editorViewportWidth, (GLsizei)editorViewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, editorTexture, 0);
        
        glBindRenderbuffer(GL_RENDERBUFFER, editorRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)editorViewportWidth, (GLsizei)editorViewportHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, editorRBO);
    
        // Check framebuffer completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            LogError("Editor framebuffer is not complete after resize!");
        }
        
        // Unbind everything
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        
        LogNormal(std::format("Resized Editor Viewport to {}x{}", width, height));
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

     Mesh::Mesh(string name, vector<Texture> textures, string shaderProgramName)
     {
         if (shaderPrograms.find(shaderProgramName) == shaderPrograms.end()) {
             LogError(format("Invalid Shader Program: {}, has the shader been initalized?", shaderProgramName));
             return;
         }

         if (meshPaths.find(name) == meshPaths.end()) {
             LogError(format("Invalid Mesh: {}", name));
             return;
         }
         auto& pathDef = meshPaths[name];
		 this->name = name;
		 this->path = pathDef.path;
         this->vertices = pathDef.vertices;
         this->indices = pathDef.indices;
         this->textures = textures;


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

    
     void Mesh::Draw(glm::mat4 model,glm::mat4 view,glm::mat4 proj) {
		 GetTextures();

         shaderProgram->setMat4("projection", proj);
         shaderProgram->setMat4("view", view);
         shaderProgram->setMat4("model", model);
		 //Bind VAO and draw mesh
		 glBindVertexArray(VAO);
         glDrawElements(GL_TRIANGLES, static_cast<GLuint>(indices.size()), GL_UNSIGNED_INT, 0);


         
		 //Unbind vao and cleanup 
         glBindVertexArray(0); 
		 glActiveTexture(GL_TEXTURE0); 

	 }

     void Mesh::GetTextures() {
         shaderProgram->Use();
         GLuint diffuse = 0, specular = 0, normal = 0, height = 0;
         for (GLuint i = 0; i < textures.size(); i++) {
             glActiveTexture(GL_TEXTURE0 + i);

             string number;
             string name;
             TextureType type = textures[i].type;

             switch (type) {
             case DIFFUSE:
                 number = std::to_string(diffuse++);
				 name = "DIFFUSE";
                 break;
             //Add when you finish the shader
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
             
             string uniformName = name + number;
             shaderProgram->setInt(uniformName, i);
			 glBindTexture(GL_TEXTURE_2D, textures[i].id);
         }
		
		 //Set shader material properties
         shaderProgram->setVec3("DIFFUSE_COLOR", material.diffuse);

         //This is just to get the shader working, please fix it since is not performant
         std::chrono::time_point<std::chrono::system_clock> now =
             std::chrono::system_clock::now();
         int time = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		 //LogNormal(std::format("Time: {}", float(time % 1000000)));
         shaderProgram->setFloat("TIME", 0.0f);
     }
#pragma endregion

#pragma region Textures
Texture::Texture(string name, TextureType type) {
	this->name = name;
    if(texturePaths.find(name) == texturePaths.end()) {
        LogError(std::format("Texture {} not found!", name));
        return;
	}

    this->path = texturePaths[name].path;
    this->type = type;
    id = 0;

    // Flip texture vertically for OpenGL
    stbi_set_flip_vertically_on_load(true);
    
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data == nullptr) {
        LogError(std::format("Failed to load texture: {}", path));
        return;
    }
    
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//For the look of the engine we go full linear filtering :D
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    if(nrChannels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    LogNormal(std::format("Loaded texture: {} with id: {}", path, id));
    stbi_image_free(data);
}
#pragma endregion

#pragma region Default Mesh Functions
std::shared_ptr<Mesh> CreateCube(string shaderProgramName, vector<Texture> textures) {
    return std::make_shared<Mesh>(PRIMATIVE_CUBE, textures, shaderProgramName);
}
std::shared_ptr<Mesh> CreateUVCube(string shaderProgramName, vector<Texture> textures) {
    if (textures.size() == 0)
        textures.push_back(Texture(PRIMATIVE_UVCUBE, TextureType::DIFFUSE));
    return std::make_shared<Mesh>(PRIMATIVE_UVCUBE, textures, shaderProgramName);
}
std::shared_ptr<Mesh> CreateCylinder(string shaderProgramName, vector<Texture> textures) {
    if (textures.size() == 0)
        textures.push_back(Texture(PRIMATIVE_CYL, TextureType::DIFFUSE));
    return std::make_shared<Mesh>(PRIMATIVE_CYL, textures, shaderProgramName);
}
std::shared_ptr<Mesh> CreatePlane(string shaderProgramName, vector<Texture> textures) {
    if (textures.size() == 0)
        textures.push_back(Texture(PRIMATIVE_PLANE, TextureType::DIFFUSE));
    return std::make_shared<Mesh>(PRIMATIVE_PLANE, textures, shaderProgramName);
}
std::shared_ptr<Mesh> CreatePyramid(string shaderProgramName, vector<Texture> textures) {
    if (textures.size() == 0)
        textures.push_back(Texture(PRIMATIVE_PYRAMID, TextureType::DIFFUSE));
    return std::make_shared<Mesh>(PRIMATIVE_PYRAMID, textures, shaderProgramName);
}
#pragma endregion


}
