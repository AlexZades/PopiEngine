#pragma once  
#include <iostream>  
#include <components.h>  
#include <vector>
#include <memory>
#include <entity.h>
#include <utils.h>
using namespace PopiEngine::ECS;
using namespace PopiEngine::Logging;
using std::string, std::vector;

namespace PopiEngine::ECS {

	vector < std::shared_ptr<Entity>> entities = vector<std::shared_ptr<Entity>>();

#pragma region Entity
	Entity::Entity(string _name) {
		name = _name;
	}

	Entity::~Entity() {
		//Cleanup components
		transform.reset();
		meshRenderer.reset();
		directionalLight.reset();
		
	}
#pragma region  attachMethods
	void Entity::AttachTransform() {
		transform = std::make_shared<Transform>();
		LogNormal(std::format("Attaching Transform to entity: {}", name));
	}
	void Entity::AttachTransform(std::shared_ptr<Transform> t) {
		transform = t;
		LogNormal(std::format("Attaching Transform to entity: {}", name));
	}	
	void Entity::AttachMesh() {
		meshRenderer = std::make_shared<MeshRenderer>();
		LogNormal(std::format("Attaching MeshRenderer to entity: {}", name));
	}
	void Entity::AttachMesh(std::shared_ptr<MeshRenderer> mr) {
		meshRenderer = mr;
		LogNormal(std::format("Attaching MeshRenderer to entity: {}", name));
	}
	void Entity::AttachMesh(GLuint meshID) {
		meshRenderer = std::make_shared<MeshRenderer>();
		meshRenderer->meshID = meshID;
		LogNormal(std::format("Attaching MeshRenderer with ID {} to entity: {}", meshID, name));
	}
	void Entity::AttachDirectionalLight() {
		directionalLight = std::make_shared<DirectionalLight>();
		LogNormal(std::format("Attaching DirectionalLight to entity: {}", name));
	}
	void Entity::AttachDirectionalLight(std::shared_ptr<DirectionalLight> dl) {
		directionalLight = dl;
		LogNormal(std::format("Attaching DirectionalLight to entity: {}", name));
	}	
	void Entity::AttachPointLight() {
		pointLight = std::make_shared<PointLight>();
		LogNormal(std::format("Attaching PointLight to entity: {}", name));
	}
	void Entity::AttachPointLight(std::shared_ptr<PointLight> pl) {
		pointLight = pl;
		LogNormal(std::format("Attaching PointLight to entity: {}", name));
	}
	void Entity::AttachCamera() {
		camera = std::make_shared<Camera>();
		LogNormal(std::format("Attaching Camera to entity: {}", name));
	}
	void Entity::AttachCamera(std::shared_ptr<Camera> cam) {
		camera = cam;
		LogNormal(std::format("Attaching Camera to entity: {}", name));
	}
	void Entity::AttachProceduralTerrain() {
		proceduralTerrain = std::make_shared<ProceduralTerrain>();
		LogNormal(std::format("Attaching ProceduralTerrain to entity: {}", name));
	}
	void Entity::AttachProceduralTerrain(std::shared_ptr<ProceduralTerrain> pt) {
		proceduralTerrain = pt;
		LogNormal(std::format("Attaching ProceduralTerrain to entity: {}", name));
	}

#pragma endregion
	ActiveComponents Entity::GetActiveComponents() {
		int components = 0;
		if (transform != nullptr) 
			components |= ActiveComponents::TRANSFORM;
		if (meshRenderer != nullptr) 
			components |= ActiveComponents::MESH_RENDERER;
		if (directionalLight != nullptr)
			components |= ActiveComponents::DIRECTIONAL_LIGHT;
		if (pointLight != nullptr)
			components |= ActiveComponents::POINT_LIGHT;
		if (camera != nullptr)
			components |= ActiveComponents::CAMERA;
		attachedComponents = static_cast<ActiveComponents>(components);
		return attachedComponents;
	}	
#pragma endregion

#pragma region EntityManager

	EntityManager::EntityManager() {
		LogNormal("Initializing Entity Manager");
		entities = vector<std::shared_ptr<Entity>>();
	}
	std::shared_ptr<Entity> EntityManager::InstatiateEntity(string name) {
		auto entity = std::make_shared<Entity>(name);
		entities.push_back(entity);
		LogNormal(std::format("Instantiated entity: {}", name));
		return entity;
	}
	void EntityManager::DestroyEntity(std::shared_ptr<Entity> entity) {
	//Implement Later
	}
#pragma endregion

}