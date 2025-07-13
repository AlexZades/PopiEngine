#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <fastNoiseLite.h>

namespace PopiEngine::Terrain
{
	struct GeneratorParams {
		float frequency = 0.01f; // Frequency of the noise
		int octaves = 4; // Number of octaves for the noise
		float lacunarity = 2.0f; // Lacunarity for the noise
		float persistence = 0.5f; // Persistence for the noise
		float heightScale = 10.0f; // Scale of the height
	};

	struct TerrainFeatures {
		int xSize = 100; // Size of the terrain in the x direction
		int ySize = 100; // Size of the terrain in the y direction

		int chunkSize = 128; // Size of each chunk
	};

	class TerrainGenerator {
	public:
		TerrainFeatures features;
		GeneratorParams params;
		

	};
}