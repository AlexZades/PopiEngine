#pragma once
#include <entity.h>
#include <vector>
#include <memory>
#include <components.h>
#include <json.hpp>

using namespace PopiEngine::ECS;
using std::string, std::vector, std::shared_ptr;
using json = nlohmann::json;
namespace PopiEngine::Importer {

	struct Scene
	{
		string name;
		vector<std::shared_ptr<PopiEngine::ECS::Entity>> sceneEntities;

		
		void Save();
		void Load();

		json toJson();
		void FromJson(const json& _json);
		Scene(string name ="scene") {
			this->name = name;
			sceneEntities = entities; //Initialize with the global entities vector
		}
	};

} 