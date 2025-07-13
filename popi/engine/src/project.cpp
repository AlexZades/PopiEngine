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
        graphicsCore = new GraphicsCore();
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

        auto etest = Entity("Cube");
		etest.AttachTransform();
        etest.AttachMesh(meshId);
        //Mesh cube = Mesh()
     
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // Use float literals to avoid warnings

       
        while (!glfwWindowShouldClose(graphicsCore->GetWindow())) {
			graphicsCore->FrameStart();
			uiCore->NewFrame();
            
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            void OnUpdate();
            static bool showDemo = false;
            ImGui::Begin("Example");
            if (ImGui::Button("Show/Hide ImGui demo"))
                showDemo = !showDemo;
            ImGui::End();
            if (showDemo)
                ImGui::ShowDemoWindow(&showDemo);

            ImGui::Render();

			graphicsCore->Draw();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        OnQuit();
        return 0;
        

	}

    void Project::OnQuit() {
        LogNormal("ぽっぴ Engine is Closing! (・_・;)");

		//Cleanup (be careful with order of deletion)
        delete uiCore;
        delete graphicsCore;
	}

    void Project::OnUpdate() {
        
        uiCore->NewFrame();

	}

    void Project::Quit() {
        OnQuit();
        glfwTerminate();
		exit(0);
    }
}