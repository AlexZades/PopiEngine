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
using namespace PopiEngine::Graphics;
using namespace PopiEngine::UI;
using std::string, std::runtime_error;

 // Initialize settings
void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, GLchar const* message,
    void const* user_param) {
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return;

    auto const src_str = [source]() {
        switch (source) {
        case GL_DEBUG_SOURCE_API:
            return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "WINDOW SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "SHADER COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "THIRD PARTY";
        case GL_DEBUG_SOURCE_APPLICATION:
            return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER:
            return "OTHER";
        default:
            return "UNKNOWN SOURCE";
        }
        }();

    auto const type_str = [type]() {
        switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "PERFORMANCE";
        case GL_DEBUG_TYPE_MARKER:
            return "MARKER";
        case GL_DEBUG_TYPE_OTHER:
            return "OTHER";
        default:
            return "UNKNOWN TYPE";
        }
        }();

    auto const severity_str = [severity]() {
        switch (severity) {
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "NOTIFICATION";
        case GL_DEBUG_SEVERITY_LOW:
            return "LOW";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "MEDIUM";
        case GL_DEBUG_SEVERITY_HIGH:
            return "HIGH";
        default:
            return "UNKNOWN SEVERITY";
        }
        }();
    std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id
        << ": " << message << '\n';
}

int main(int argc, char* argv[]) {
    PopiEngine::Settings settings;

    if (!glfwInit()) 
        throw std::runtime_error("Error initializing glfw");
    
    auto window = PopiEngine::Graphics::InitalizeWindow(settings.windowWidth, settings.windowHeight, settings.windowName);
    
    glfwSwapInterval(1);
    PopiEngine::UI::InitializeImGui(window);
    if (!gladLoaderLoadGL())
        throw std::runtime_error("Error initializing glad");


    glEnable(GL_CULL_FACE);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(message_callback, nullptr);

	//Initialize shaders
    PopiEngine::Graphics::InitalizeShaders();

    auto vertexShader = PopiEngine::Graphics::shaders[settings.defaultVertexShader];
	std::cout << std::format("Using vertex shader: {}\n", vertexShader ->shaderID);
    auto fragmentShader = PopiEngine::Graphics::shaders[settings.defaultFragmentShader];
    std::vector<std::shared_ptr<Shader>> defaultShaders = { vertexShader, fragmentShader };
    ShaderProgram shaderprogram(defaultShaders);
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
	//shaderprogram.Use();
	shaderprogram.Use();
    glClearColor(1, 1, 1, 1);

    while (!glfwWindowShouldClose(window)) {
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

        glfwSwapBuffers(window);
        glClear(GL_COLOR_BUFFER_BIT);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}