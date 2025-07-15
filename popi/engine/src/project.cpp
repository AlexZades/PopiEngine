#define GLFW_INCLUDE_NONE

#include <chrono>
#include <glad/gl.h> //always include glad before glfw
#include <glm/glm.hpp>
#include <GLFW/glfw3.h> 
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <thread>
#include <graphics.h>
#include <camera.h>
#include <settings.h>
#include <ui.h>
#include <project.h>
#include <entity.h>
#include <components.h>
#include <utils.h>
#include <fstream>
#include <tiny_obj_loader.h>
#include <importer.h>
#include <popiinput.h>
#include <scripts.h>    
using namespace PopiEngine::Graphics;
using namespace PopiEngine::UI;
using namespace PopiEngine::ECS;
using namespace PopiEngine::Logging;
using namespace PopiEngine::Input;

using std::string, std::runtime_error, std::format, std::ifstream, std::stringstream;

// Initialize settings




namespace PopiEngine {

    
	float deltaTime = 0.0f; // Global delta time variable
    std::shared_ptr<EntityManager> entityManagerRef; //PLEASE make entity manager a singlton 


    void Project::OnStart(){
        try {
            ifstream versionFile(format("{}/ver", RESOURCES));
            stringstream verBuffer;
			verBuffer << versionFile.rdbuf();
            LogNormal(format("Engine Version: {}", verBuffer.str()));
        }
        catch (const std::exception& e) {
            LogError(format("Failed to read version file: {}", e.what()));
		}
		LogNormal("ぽっぴ Engine is Starting! O=('-'Q)");

		//Initialize cores
        graphicsCore = new GraphicsCore(settings.editorMode);
		entityManager = std::make_shared<EntityManager>(); //Initiaize Before UI Core
        entityManagerRef = entityManager; //this is a sin against c++ programing
        inputCoreRef->LinkCallbacks(graphicsCore->GetWindow()); //Link input callbacks to the window
        uiCore = new UICore(graphicsCore->GetWindow());
		Importer::currentScene = Scene(); //Initialize the current scene
		
	}

    int Project::Run() {
        OnStart();

        
        //Initialize shaders
        auto defaultShaderProgram = InitalizeShader("unlit");
        
        //Link the camera to the entity manager

        currentScene.Load(scenePaths.begin()->second.path);
     
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // Use float literals to avoid warnings

        auto lastFrameTime = std::chrono::steady_clock::now();
		auto now = std::chrono::steady_clock::now();
        while (!glfwWindowShouldClose(graphicsCore->GetWindow())) {
			deltaTime = std::chrono::duration<float>(now - lastFrameTime).count();
            
			graphicsCore->FrameStart();
			uiCore->NewFrame();
            if(settings.editorMode)
				OnEditorUpdate();
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            OnUpdate();

            ImGui::Render();

			graphicsCore->Draw();
            //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        OnQuit();
        return 0;
        

	}

    void Project::OnEditorUpdate() {
        uiCore->DrawEditor(graphicsCore->GetEditorTexture());
    }

    void Project::OnFixedUpdate() {
        //Fixed update logic can go here
        //Currently not used
	}

    void Project::OnQuit() {
        LogNormal("ぽっぴ Engine is Closing! (・_・;)");

		//Cleanup (be careful with order of deletion)
        delete uiCore;
        delete graphicsCore;
	}

    void Project::OnUpdate() {
		PopiEngine::Scripts::OnUpdate(); //Call the scripts update method
    }

    void Project::Quit() {
        OnQuit();
        glfwTerminate();
		exit(0);
    }
}