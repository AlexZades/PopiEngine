#pragma once  
#include <iostream>  
#include <components.h>  
#include <vector>
#include <memory>
#include <entity.h>
using namespace PopiEngine::Importer;

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
        std::shared_ptr<ScriptHook> scriptHook; //Make this a vector later
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
        void AttachScriptHook();
        void AttachScriptHook(std::shared_ptr<ScriptHook> scriptHook);
        void RemoveComponenet(ActiveComponents target);
		//Bit mask to check which components are active
#pragma endregion
        ActiveComponents GetActiveComponents();
    private:   
        ActiveComponents attachedComponents;  
      
    };  

    extern vector <std::shared_ptr<Entity>> entities;

    //We need to refactor this to singleton instead of using externs
    class EntityManager {
    public:
        
		
        EntityManager();

        std::shared_ptr<Entity> InstatiateEntity(string name);

		void DestroyEntity(std::shared_ptr<Entity> entity);

		void OnNewScene();
		void OnSceneLoad();


    };

    
}