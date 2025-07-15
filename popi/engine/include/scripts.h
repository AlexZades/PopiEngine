#pragma once

#include <s_playerInput.h>

#include <project.h>

namespace PopiEngine::Scripts
{
	PlayerInput playerInput;
	void OnUpdate() {
		playerInput.OnUpdate();
	}
}