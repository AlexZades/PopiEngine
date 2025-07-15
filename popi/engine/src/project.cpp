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
using namespace PopiEngine::Graphics;
using namespace PopiEngine::UI;
using namespace PopiEngine::ECS;
using namespace PopiEngine::Logging;

using std::string, std::runtime_error, std::format, std::ifstream, std::stringstream;

// Initialize settings




namespace PopiEngine {

    

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

       
        while (!glfwWindowShouldClose(graphicsCore->GetWindow())) {
			graphicsCore->FrameStart();
			uiCore->NewFrame();
            if(settings.editorMode)
				OnEditorUpdate();
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            OnUpdate();

            ImGui::Render();

			graphicsCore->Draw();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
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

    }

    void Project::Quit() {
        OnQuit();
        glfwTerminate();
		exit(0);
    }
}