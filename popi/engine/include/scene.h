#pragma once
#include <entity.h>
#include <vector>
#include <memory>
#include <components.h>
using namespace PopiEngine::ECS;
using std::string, std::vector, std::shared_ptr;

namespace PopiEngine::Importer {

	struct Scene
	{
		string name;
		std::shared_ptr<vector<std::shared_ptr<Entity>>> entities;

		
		void Save(string name);
		void Load(string name);
	};
} 