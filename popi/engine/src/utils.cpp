
#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
using std::string;

// Logging utilities for PopiEngine
namespace PopiEngine::Logging {

	void LogNormal(const string& message) {
		std::cout << "[INFO]: " << message << std::endl;
	}

	void LogWarning(const string& message) {
		std::cout << "[WARNING]: " << message << std::endl;
	}
	void LogError(const string& message) {
		std::cerr << "[ERROR]: " << message << std::endl;
	}

	void LogGLError(char infoLog[]) {
		std::cerr << "[OpenGLError]: " << infoLog << std::endl;
	}

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
}