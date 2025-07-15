#pragma once

#include <graphics.h>
#include <settings.h>
#include <ui.h>
#include <entity.h>

using namespace PopiEngine::Graphics;
using namespace PopiEngine;
using namespace PopiEngine::ECS;
using namespace PopiEngine::UI;
//Container for main program of popi engine
//Modify project.cpp to change the behavior
namespace PopiEngine {
	extern std::shared_ptr<EntityManager> entityManagerRef;
	extern float deltaTime;
	class Project {
	public:
		Project() = default;

	
		virtual void OnStart();
		virtual void OnUpdate();
		virtual void OnFixedUpdate();
		virtual void OnQuit();
		virtual void OnEditorUpdate();
		int Run();
		void Quit();

		UICore* uiCore;
		GraphicsCore* graphicsCore;
		std::shared_ptr <EntityManager> entityManager;
		Settings settings = Settings();
	private:
		float lastFrameTime = 0;
	};
	

}