#pragma once  
#include <iostream>  
#include <components.h>  
using namespace PopiEngine::ECS;  
using std::string;

namespace PopiEngine::ECS {  
      
    class Entity {
    public:
		string name;
        std::shared_ptr<Transform> transform;
        std::shared_ptr<MeshRenderer> meshRenderer;
        std::shared_ptr<DirectionalLight> directionalLight;

        Entity(string _name = "") {
			name = _name;
        }
        ~Entity(){
        
        }
  
        void AttachTransform() {  
            transform = std::make_shared<Transform>();  
        }  
        void AttachMesh() {  
            meshRenderer = std::make_shared<MeshRenderer>();  
        }  
        void AttachDirectionalLight() {  
            directionalLight = std::make_shared<DirectionalLight>();  
        }  
    private:   
        ActiveComponents attachedComponents;  
  
    };  
}