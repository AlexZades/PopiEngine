#pragma once

#include <graphics.h>
#include <settings.h>
#include <ui.h>

using namespace PopiEngine::Graphics;
using namespace PopiEngine;
using namespace PopiEngine::UI;
//Container for main program of popi engine
//Modify project.cpp to change the behavior
namespace PopiEngine {
	class Project {
	public:
		Project() = default;

		virtual void OnStart();
		virtual void OnUpdate();
		virtual void OnQuit();

		int Run();
		void Quit();

		UICore* uiCore;
		GraphicsCore* graphicsCore;
		Settings settings = Settings();
	};
	

}