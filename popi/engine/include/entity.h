#pragma once  
#include <iostream>  
#include <components.h>  
#include <vector>
#include <memory>
#include <entity.h>

using namespace PopiEngine::ECS;  
using std::string, std::vector;

namespace PopiEngine::ECS {  
      
	

    class Entity {
    public:
		string name;
		bool isActive = true;
        std::shared_ptr<Transform> transform;
        std::shared_ptr<MeshRenderer> meshRenderer;
        std::shared_ptr<DirectionalLight> directionalLight;
		std::shared_ptr<PointLight> pointLight;
		std::shared_ptr<Camera> camera;
		std::shared_ptr<ProceduralTerrain> proceduralTerrain;

        Entity(string _name = "entity");
        ~Entity();
        
#pragma region  attachMethods
        void AttachTransform();
        void AttachTransform(std::shared_ptr<Transform> t);
        void AttachMesh();
        void AttachMesh(std::shared_ptr<MeshRenderer> mr);
        void AttachMesh(GLuint meshID);
        void AttachDirectionalLight();
        void AttachDirectionalLight(std::shared_ptr<DirectionalLight> dl);
        void AttachPointLight();
		void AttachPointLight(std::shared_ptr<PointLight> pl);
		void AttachCamera();
		void AttachCamera(std::shared_ptr<Camera> camera);
		void AttachProceduralTerrain();
		void AttachProceduralTerrain(std::shared_ptr<ProceduralTerrain> pt);

        void RemoveComponenet(ActiveComponents target);
		//Bit mask to check which components are active
#pragma endregion
        ActiveComponents GetActiveComponents();
    private:   
        ActiveComponents attachedComponents;  
      
    };  

    extern vector <std::shared_ptr<Entity>> entities;

    class EntityManager {
    public:
        
        EntityManager();

        std::shared_ptr<Entity> InstatiateEntity(string name);
		void DestroyEntity(std::shared_ptr<Entity> entity);
    };
}