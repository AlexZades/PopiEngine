#pragma once

#include <project.h>
#include <entity.h>
#include <popiinput.h>
#include <components.h>
const string scriptName = "PlayerInput";
using namespace PopiEngine::ECS;
using namespace PopiEngine::Input; 
namespace PopiEngine::Scripts {

	const float movementSpeed = 160000.0f; // Speed of movement
	class PlayerInput{
	public:
		void OnUpdate() {
			for (auto const& entity : PopiEngine::ECS::entities) {
			
				auto activeComponents = entity->GetActiveComponents();
				if (activeComponents & ActiveComponents::SCRIPT_HOOK && activeComponents & ActiveComponents::TRANSFORM) {
					auto& script = entity->scriptHook;
					if (script->isActive && script->scriptName == scriptName) {
						PlayerMovement(entity);
					}
				}
			}

		}

		void PlayerMovement(std::shared_ptr<Entity> entity) {
			if(inputCoreRef->keysPressed.count(GLFW_KEY_W)) 
				entity->transform->position.z -= movementSpeed *deltaTime;
			if (inputCoreRef->keysPressed.count(GLFW_KEY_S))
				entity->transform->position.z += movementSpeed * deltaTime;
			if (inputCoreRef->keysPressed.count(GLFW_KEY_A))
				entity->transform->position.x -= movementSpeed * deltaTime;
			if (inputCoreRef->keysPressed.count(GLFW_KEY_D))
				entity->transform->position.x += movementSpeed * deltaTime;

		}
	};
}