#include "ui.h"
#include "utils.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <entity.h>
#include <graphics.h>
#include <components.h>
#include <glm/gtc/type_ptr.hpp>
#include <importer.h>
#include <project.h>
using namespace PopiEngine::Logging;
using namespace PopiEngine::ECS;
using namespace PopiEngine::Graphics;
using namespace PopiEngine::Importer;
namespace PopiEngine::UI
{

	UICore::UICore(GLFWwindow* window) {
		LogNormal("Initializing Ui Core");
		_window = window;
		InitializeImGui();
	}
	UICore::~UICore() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void UICore::InitializeImGui() {
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(_window, true);
		ImGui_ImplOpenGL3_Init("#version 450");
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui::StyleColorsDark();
	}	

	//Call one per fram to update ImGui state
	void UICore::NewFrame() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}


	void UICore::UpdateEntites() {
		if(entities.size() -1 < selectedEntityIndex) {
			selectedEntityIndex = 0; // Reset to 0 if the index is out of bounds
		}
	}

	void UICore::DrawMenuBar() {
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				FileMenu();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				EditMenu();
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();

		//File bar dialogs (save dialog)
		if (showSaveBox)
			ImGui::OpenPopup("SaveDialog");
		if (ImGui::BeginPopup("SaveDialog"))
		{
			//Imgui doesnt like strings and to use the nativly
			//I need to setup a callback so im doing this hacky stuff
			static char saveName[128];
			strncpy(saveName, currentScene.name.c_str(), sizeof(saveName) - 1);
			saveName[sizeof(saveName) - 1] = '\0';

			ImGui::InputText("Scene Name", saveName, IM_ARRAYSIZE(saveName));
			currentScene.name = saveName;
			if (ImGui::Button("Save")) {
				 
				currentScene.Save();
				ImGui::CloseCurrentPopup();
				showSaveBox = false; // Hide the save dialog after saving
			} ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
				showSaveBox = false;
			}
			ImGui::EndPopup();

		}
	}

	void UICore::FileMenu() {
		if (ImGui::MenuItem("New Scene")) {
			currentScene.New(); 
		}
		if (ImGui::BeginMenu("Load Scene"))
			{
				for (const auto& [key, value] : scenePaths)
				{
					if (ImGui::MenuItem(key.c_str()))
					{
						currentScene.Load(value.path);
					}
				}
				ImGui::EndMenu();
			}
		
		if (ImGui::MenuItem("Save Scene"))
		{
			showSaveBox = true; // Show the save dialog when the menu item is clicked
		}
		
	}
	
	void UICore::EditMenu() {
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::BeginMenu("Entity"))
			{
				if (ImGui::MenuItem("Empty"))
				{
					entities.push_back(std::make_shared<Entity>("New Entity"));
				}
				if (ImGui::MenuItem("Cube"))
				{
					auto mesh = activeGraphicsCore->LinkMesh(CreateCube("unlit"));
					auto entity = std::make_shared<Entity>("Cube");
					entity->AttachMesh(mesh);
					entity->AttachTransform(std::make_shared<Transform>());
					entities.push_back(entity);
				}
				if (ImGui::MenuItem("UVCube"))
				{
					auto mesh = activeGraphicsCore->LinkMesh(CreateUVCube("unlit"));
					auto entity = std::make_shared<Entity>("UVCube");
					entity->AttachMesh(mesh);
					entity->AttachTransform(std::make_shared<Transform>());
					entities.push_back(entity);
				}
				if (ImGui::MenuItem("Plane"))
				{
					auto mesh = activeGraphicsCore->LinkMesh(CreatePlane("unlit"));
					auto entity = std::make_shared<Entity>("Plane");
					entity->AttachMesh(mesh);
					entity->AttachTransform(std::make_shared<Transform>());
					entities.push_back(entity);
				}
				if (ImGui::MenuItem("Pyramid"))
				{
					auto mesh = activeGraphicsCore->LinkMesh(CreatePyramid("unlit"));
					auto entity = std::make_shared<Entity>("Pyramid");
					entity->AttachMesh(mesh);
					entity->AttachTransform(std::make_shared<Transform>());
					entities.push_back(entity);
				}
				if (ImGui::MenuItem("Cylinder"))
				{
					auto mesh = activeGraphicsCore->LinkMesh(CreateCylinder("unlit"));
					auto entity = std::make_shared<Entity>("Cylinder");
					entity->AttachMesh(mesh);
					entity->AttachTransform(std::make_shared<Transform>());
					entities.push_back(entity);
				}
				ImGui::EndMenu();
			}
			
			ImGui::EndMenu();
		}
		//if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {}
	}
	void UICore::InspectorMenu()
	{
		AddComponentMenu();
	}

	void UICore::AddComponentMenu()
	{
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("Add Component");
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
			entityManagerRef->DestroyEntity(entities[selectedEntityIndex]);
		if (ImGui::BeginPopup("Add Component"))
		{
			if (ImGui::MenuItem("Transform"))
			{
				if (!entities[selectedEntityIndex]->transform)
					entities[selectedEntityIndex]->AttachTransform(std::make_shared<Transform>());
			}
			if (ImGui::MenuItem("Camera"))
			{
				if (!entities[selectedEntityIndex]->camera)
					entities[selectedEntityIndex]->AttachCamera(std::make_shared<ECS::Camera>());
			}
			if (ImGui::MenuItem("Mesh Renderer"))
			{
				if(!entities[selectedEntityIndex]->meshRenderer) {
					// Open the mesh selection UI
					showMeshSelection = true; 
				}
				
			}
			ImGui::EndPopup();
		}

		if(showMeshSelection) {
			ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
			ImGui::SetNextWindowSize(ImVec2(300, 200));
			ImGui::Begin("Select Mesh", &showMeshSelection, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			
			// Add the mesh renderer selection UI
			AddMeshRendererMenu();
			
			ImGui::End();
		}
	}

	void UICore::AddMeshRendererMenu(){
		
				ImGui::Separator();
				vector<string> items;
				for (const auto& [key, value] : PopiEngine::Importer::meshPaths) {
					items.push_back(key);
				}
				

				if (selectedMesh >= items.size()) {
					selectedMesh = 0;	
				}
				
				if (ImGui::BeginCombo("Mesh Type", items[selectedMesh].c_str()))
				{
					for (int i = 0; i < items.size(); i++)
					{
						const bool selected = (selectedMesh == i);
						if (ImGui::Selectable(items[i].c_str(), selected))
							selectedMesh = i;
						if (selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				
				ImGui::Spacing();
				
				if (ImGui::Button("Create Mesh", ImVec2(-1, 0))) {
					auto id = activeGraphicsCore->LinkMesh(std::make_shared<Mesh>(items[selectedMesh], vector<Texture>(), "unlit"));
					entities[selectedEntityIndex]->AttachMesh(id);
					showMeshSelection = false; 
				}
				
				if (ImGui::Button("Cancel", ImVec2(-1, 0))) {
					showMeshSelection = false;
				}
			}

	
	void UICore::ResourcesMenu()
	{
		ImGui::Begin("Resources");
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("Resources", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Scenes"))
			{
				SceneMenu();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Textures"))
			{

				TextureMenu();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Meshes"))
			{
				MeshMenu();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Shaders"))
			{
				ShaderMenu();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		ImGui::End();
	}

	void UICore::TextureMenu()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
		ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_None, window_flags);
		for (const auto& [key,path]: texturePaths) {
			ImGui::Text(key.c_str());
		}
		ImGui::EndChild();
	}

	void UICore::ShaderMenu()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
		ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_None, window_flags);
		for (const auto& [key, path] : shaderPaths) {
			ImGui::Text(key.c_str());
		}
		ImGui::EndChild();
	}

	void UICore::MeshMenu()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
		ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_None, window_flags);
		for (const auto& [key, path] : meshPaths) {
			ImGui::Text(key.c_str());
		}
		ImGui::EndChild();
	}

	void UICore::SceneMenu()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
		ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_None, window_flags);
		for (const auto& [key, path] : scenePaths) {
			ImGui::Text(key.c_str());
		}
		ImGui::EndChild();
	}

	
	

	void UICore::DrawEditor(GLuint viewTexture) {
		 // Create a dockspace that covers the entire viewport
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		
		ImGui::Begin("DockSpace Demo", nullptr, window_flags);
		ImGui::PopStyleVar(3);
		
		// Create the dockspace
		ImGuiID dockspace_id = ImGui::GetID("Dock");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		DrawMenuBar();
		ImGui::End();
		
		DrawHeirarchy();
		DrawSceneView(viewTexture);
		DrawInspector();
		ResourcesMenu();
		ResourcePreviewMenu();
	}
	
	void UICore::DrawSceneView(GLuint viewTexture) {
		ImGui::Begin("Scene View");
		{
			ImGui::BeginChild("Render");

			// Get the current available content region size
			ImVec2 viewportSize = ImGui::GetContentRegionAvail();

			// Check if viewport size has changed and update if necessary
			static float lastWidth = 0;
			static float lastHeight = 0;
			if (viewportSize.x != lastWidth || viewportSize.y != lastHeight) {
				if (viewportSize.x > 0 && viewportSize.y > 0) {
					// Resize the editor framebuffer to match the ImGui window size
					if (PopiEngine::Graphics::activeGraphicsCore) {
						PopiEngine::Graphics::activeGraphicsCore->ResizeEditorViewport(viewportSize.x, viewportSize.y);
					}
					lastWidth = viewportSize.x;
					lastHeight = viewportSize.y;
				}
			}

			// Only render if we have valid dimensions
			if (viewportSize.x > 0 && viewportSize.y > 0) {
				ImGui::Image(
					(ImTextureID)viewTexture,
					viewportSize,
					ImVec2(0, 1),
					ImVec2(1, 0)
				);
			}

			ImGui::EndChild();
		}
		ImGui::End();
	}
	void UICore::DrawHeirarchy() {
		UpdateEntites();
		ImGui::Begin("Heirachy", &editorTools, ImGuiWindowFlags_MenuBar);

		ImGui::BeginChild("Entites");
		ImGui::BeginListBox("##listbox", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y)); {
			for (int i = 0; i < entities.size(); i++) {
				const auto& entity = entities[i];
				const bool is_selected = (selectedEntityIndex == i);
				if (entity->isActive) {
					if (ImGui::Selectable(format("[{}]{}", i, entity->name).c_str(), is_selected)){
						selectedEntityIndex = i; // Update the selected entity index
					}
				}
				else
				{
					if (ImGui::Selectable(format("[{}]{} (disabled)", i, entity->name).c_str(), is_selected)) {
						selectedEntityIndex = i; // Update the selected entity index
					}
				}
			}
		}
		ImGui::EndListBox();
		ImGui::EndChild();
		ImGui::End();
	}
	/// <summary>
	/// Draw the inspector for the currently selected entity.
	/// </summary>
	void UICore::DrawInspector() {
		
		//Safety check
		if (selectedEntityIndex < 0 || selectedEntityIndex >= entities.size()) {
			return; 
		}
		auto& entity = entities[selectedEntityIndex];
		ActiveComponents activeComponents = entity->GetActiveComponents();
		ImGui::Begin("Inspector");
		InspectorMenu();
        ImGui::Checkbox("Entity Enabled:", &entity->isActive);
		//Check which components are active and draw their gizmos
		if(activeComponents & TRANSFORM) {
			TransformGizmo(entity->transform);
		}
		if(activeComponents & MESH_RENDERER) {
			MeshRendererGizmo(entity->meshRenderer);
		}
		if(activeComponents & CAMERA) {
			CameraGizmo(entity->camera);
		}

		ImGui::End();
	}

#pragma region Component Spesific UI
	void UICore::ResourcePreviewMenu()
	{
		ImGui::Begin("Resource Preview");
		ImGui::End();
	}
	void UICore::TransformGizmo(std::shared_ptr<Transform> transform)
	{
		//To modify the transform of the selected entity we need to unpack
		//the glm::vec3 to a floats so imgui can modify them
		ImGui::SeparatorText("Transform"); ImGui::SameLine(); if(ImGui::SmallButton("X##transfomr")) {
			entities[selectedEntityIndex]->RemoveComponenet(ActiveComponents::TRANSFORM);
		}
		ImGui::InputFloat3("Position", glm::value_ptr(transform->position));
		ImGui::InputFloat3("Rotation", glm::value_ptr(transform->rotation));
		ImGui::InputFloat3("Scale", glm::value_ptr(transform->scale));
	}

	void UICore::MeshRendererGizmo(std::shared_ptr<MeshRenderer> meshRenderer)
	{
		ImGui::SeparatorText("Mesh Renderer");ImGui::SameLine(); if (ImGui::SmallButton("X##Mesh")) {
			entities[selectedEntityIndex]->RemoveComponenet(ActiveComponents::MESH_RENDERER);
		}
		if (meshRenderer) {
			ImGui::Text(format("Mesh ID:{}", meshRenderer->meshID).c_str());
			
			auto mesh = activeGraphicsCore->activeMeshes[meshRenderer->meshID];
			if(!mesh) {
				ImGui::Text("Mesh not found!");
				return;
			}
			ImGui::Checkbox("Transparent:", &meshRenderer->isTransparent);
			ImGui::Text(format("{}", mesh->path).c_str());

			if(ImGui::Button("Add Texture")) 
				ImGui::OpenPopup("Add Texture");
				//ImGui::OpenPopup("Add Component");
				
				MeshGizmoAddTexture(meshRenderer);
			

			if (mesh->textures.size() > 0) {
				//Display the textures used by the mesh
				if (ImGui::TreeNode("Texture")) {
					for (const auto meshTexture : mesh->textures) {
						string path = meshTexture.path;
						string name = meshTexture.name;
						ImGui::Text(name.c_str());
						ImGui::Image((ImTextureID)meshTexture.id, ImVec2(64, 64));
						//Button to remove the texture
						if (ImGui::SmallButton(format("Remove Texture##{}", name).c_str())) {
							//Remove the texture from the mesh 
							mesh->textures.erase(std::remove_if(mesh->textures.begin(), mesh->textures.end(),
								[&](const Texture& t) { return t.name == name; }), mesh->textures.end());
						}
						ImGui::Text(path.c_str());
					}
					ImGui::TreePop();
				}
			}
			if (mesh->shaderProgram) {
				ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(mesh->material.diffuse));
				ImGui::Text(format("Shader [{}]: {}",mesh->shaderProgram->GetId(), mesh->shaderProgram->name).c_str());
				//Add more shader properties if needed
			}
			
			
			else {
				ImGui::Text("No Shader Program linked to this mesh!");
			}
			
			
		}
	}

	void UICore::MeshGizmoAddTexture(std::shared_ptr<MeshRenderer> meshRenderer)
	{
		auto mesh = activeGraphicsCore->activeMeshes[meshRenderer->meshID];
		if (!mesh) {
			return;
		}
		if (ImGui::BeginPopup("Add Texture"))
		{
			
			for(const auto& [name, path] : texturePaths) {
				if (ImGui::MenuItem(name.c_str())) {
					//Add logic here to change texture type once its setup 
					auto texture = Texture(name, TextureType::DIFFUSE);
					mesh->textures.push_back(texture);
				}
			}
			
			ImGui::EndPopup();
		}
	}

	void UICore::CameraGizmo(std::shared_ptr<ECS::Camera> camera)
	{
	ImGui::SeparatorText("Camera");ImGui::SameLine(); if (ImGui::SmallButton("X##cam")) {
		entities[selectedEntityIndex]->RemoveComponenet(ActiveComponents::CAMERA);
	}
		if (camera) {
			int selected = camera->mode;
			const char* names[] = { "Perspective", "Orthographic" };
			ImGui::Combo("Mode", &selected, names, IM_ARRAYSIZE(names), 4);
			camera->mode = static_cast<CameraMode>(selected);
			if(camera->mode == CameraMode::ORTHOGRAPHIC) {
				ImGui::InputFloat("Orthographic Size", &camera->orthographicSize);
			}
			else {
				ImGui::InputFloat("FOV", &camera->fov);
				
			}
			ImGui::InputFloat("Near Plane", &camera->nearPlane);
			ImGui::InputFloat("Far Plane", &camera->farPlane);
		}

	}

	void UICore::PontLightGizmo(std::shared_ptr<PointLight> pointLight)
	{
		ImGui::SeparatorText("Point Light");ImGui::SameLine(); if (ImGui::SmallButton("X##pointLight")) {
			entities[selectedEntityIndex]->RemoveComponenet(ActiveComponents::POINT_LIGHT);
		}
		if (pointLight) {
			//Bsic light properties
			ImGui::InputFloat("Intensity:", &pointLight->intensity);
			ImGui::ColorEdit3("Ambient", glm::value_ptr(pointLight->ambient));
			ImGui::ColorEdit3("Diffuse", glm::value_ptr(pointLight->diffuse));
			ImGui::ColorEdit3("Specular", glm::value_ptr(pointLight->specular));

			// Attenuation parameters
			ImGui::Text("Attenuation Parameters");
			ImGui::InputFloat("Constant:", &pointLight->constant);
			ImGui::InputFloat("Linear:", &pointLight->linear);
			ImGui::InputFloat("Quadratic:", &pointLight->quadratic);
		}
	}

	void UICore::DirectionalLightGizmo(std::shared_ptr<DirectionalLight> directionalLight)
	{
	}

#pragma endregion


}

