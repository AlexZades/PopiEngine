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
using namespace PopiEngine::Graphics;
using namespace PopiEngine::UI;
using namespace PopiEngine::ECS;
using namespace PopiEngine::Logging;

using std::string, std::runtime_error, std::format, std::ifstream, std::stringstream;

// Initialize settings




namespace PopiEngine {

    std::shared_ptr<Entity> testEntity;

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
		entityManager = new EntityManager(); //Initiaize Before UI Core
        uiCore = new UICore(graphicsCore->GetWindow());

	}

    int Project::Run() {
        OnStart();

        //Initialize shaders
        auto defaultShaderProgram = InitalizeShader("unlit");
        
		//Not sure why this is ambiguous??!?
        auto mainCamera = new PopiEngine::Graphics::Camera(
            glm::vec3(0.0f, 0.0f, 3.0f), // Position - moved back to see the cube better
            glm::vec3(0.0f, 1.0f, 0.0f), // Up vector
            -90.0f, // Yaw
            0.0f,   // Pitch
            2.5f,   // Speed
            0.1f    // Sensitivity
		);
		//Link the camera to the graphics core
        graphicsCore->LinkCamera(
			std::shared_ptr<PopiEngine::Graphics::Camera>(mainCamera)
		);
		Texture test = Texture("Super_Mario", TextureType::DIFFUSE);
        vector<Texture> textures = { test };
        auto meshId = graphicsCore->LinkMesh(CreateCube("unlit"));

        testEntity = entityManager->InstatiateEntity("TestEntity");
        testEntity->AttachTransform();
        testEntity->AttachMesh(meshId);

        auto meshId2 = graphicsCore->LinkMesh(CreateCube("unlit"));
        auto testEntity2 = entityManager->InstatiateEntity("TestEntity");
		auto pos = std::make_shared<Transform>(
            glm::vec3(2.0f, 0.0f, 0.0f),
            glm::vec3(0.0f,0.0f,45.0f),
            glm::vec3(0.5f,0.5f,2.0f));
        testEntity2->AttachTransform(pos);
        testEntity2->AttachMesh(meshId2);
        //Mesh cube = Mesh()
     
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
        testEntity->transform.get()->rotation.x += 0.4f;
    }

    void Project::Quit() {
        OnQuit();
        glfwTerminate();
		exit(0);
    }
}