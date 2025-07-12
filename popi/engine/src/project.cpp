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
#include <settings.h>
#include <ui.h>
#include <project.h>
#include <utils.h>
#include <fstream>
using namespace PopiEngine::Graphics;
using namespace PopiEngine::UI;
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
        graphicsCore = new GraphicsCore();
	}
	int Project::Run() {
		OnStart();

        

        glfwSwapInterval(1);
        PopiEngine::UI::InitializeImGui(graphicsCore->GetWindow());




        glEnable(GL_CULL_FACE);
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(message_callback, nullptr);

        //Initialize shaders
		auto defaultShaderProgram = InitalizeShader(settings.unlitShader);
        /**
            * Create vertex array and buffers
           */
        GLuint vao;
        glCreateVertexArrays(1, &vao);

        glEnableVertexArrayAttrib(vao, 0);
        glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE,
            offsetof(glm::vec2, x));

        glVertexArrayAttribBinding(vao, 0, 0);

        glm::vec2 vertices[] = { {-0.2, -0.2}, {-0.2, 0.2}, {0.2, 0.2}, {0.2, -0.2} };

        GLuint vbo;
        glCreateBuffers(1, &vbo);
        glNamedBufferStorage(vbo, sizeof(glm::vec2) * 4, vertices,
            GL_DYNAMIC_STORAGE_BIT);

        std::uint32_t indices[] = { 0, 2, 1, 2, 0, 3 };

        GLuint ibo;
        glCreateBuffers(1, &ibo);
        glNamedBufferStorage(ibo, sizeof(std::uint32_t) * 6, indices,
            GL_DYNAMIC_STORAGE_BIT);

        glBindVertexArray(vao);
        glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(glm::vec2));
        glVertexArrayElementBuffer(vao, ibo);
        defaultShaderProgram->Use();
        glClearColor(0, 0, 0, 0);

        while (!glfwWindowShouldClose(graphicsCore->GetWindow())) {
            glfwPollEvents();

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            static bool showDemo = false;
            ImGui::Begin("Example");
            if (ImGui::Button("Show/Hide ImGui demo"))
                showDemo = !showDemo;
            ImGui::End();
            if (showDemo)
                ImGui::ShowDemoWindow(&showDemo);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(graphicsCore->GetWindow());
            glClear(GL_COLOR_BUFFER_BIT);

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        OnQuit();
        return 0;
        

	}

    void Project::OnQuit() {
        LogNormal("ぽっぴ Engine is Closing! (・_・;)");
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(graphicsCore->GetWindow());
		
        glfwTerminate();

        //Cleanup
        delete graphicsCore;
	}

    void Project::OnUpdate() {
        
		//This Runs every frame
	}

    void Project::Quit() {
        OnQuit();
        glfwTerminate();
		exit(0);
    }
}