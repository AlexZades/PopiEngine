#pragma once
#include <iostream>
using std::string;

// Logging utilities for PopiEngine
namespace PopiEngine::Logging {

	void LogNormal(const string& message);
	void LogWarning(const string& message);
	void LogError(const string& message);
	void LogGLError(char infoLog[]);

	void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, GLchar const* message,
		void const* user_param);
}