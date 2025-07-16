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
		float lacunarity = 2.0f; 
		float persistence = 0.5f;
		float heightScale = 10.0f; 
	};

	struct TerrainFeatures {
		int xSize = 100; //Size in units
		int ySize = 100;

		int chunkSize = 128; //Vertex grid determiner
	};

	class TerrainGenerator {
	public:
		TerrainFeatures features;
		GeneratorParams params;
		

	};
}