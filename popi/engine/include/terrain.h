#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <fastNoiseLite.h>

namespace PopiEngine::Terrain
{
	struct TerrainFeatures {
		int xSize = 100; // Size of the terrain in the x direction
		int ySize = 100; // Size of the terrain in the y direction

		int chunkSize = 128; // Size of each chunk
	};

	class TerrainGenerator {


	};
}