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
		std::shared_ptr<vector<std::shared_ptr<Entity>>> entities;

		
		void Save();
		void Load();

		json ToJson();
		void FromJson(const json& _json);
	};

} 