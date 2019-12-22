#ifndef __MODULE_IMPORT_H__
#define __MODULE_IMPORT_H__

#include "Module.h"
#include "MathGeoLib\src\Algorithm\Random\LCG.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

class GameObject;
class C_Mesh;
class C_Material;
class C_Animation;
class C_Camera;
class C_Transform;
class C_ParticleSystem;
class R_Prefab;
class Component;

class Config;

class M_Import : public Module
{
public:

	M_Import(bool start_enabled = true);
	~M_Import();

	R_Prefab* ImportFile(const char* path, uint size, Uint32 ID);
	GameObject* CreateGameObjects(const aiScene*, const aiNode*, GameObject*, const char*, std::vector<GameObject*>&, std::vector<const aiMesh*>&, std::vector<const GameObject*>&);
	bool Init(Config& config); 
	bool CleanUp(); //
	update_status Update(float dt); //

	void		SaveGameObjectConfig(Config& config, std::vector<GameObject*>& gameObjects);

	GameObject* LoadGameObject(uint64 ID);
	void		LoadGameObjectConfig(Config& config, std::vector<GameObject*>& nodes);

	R_Prefab* LoadPrefabResource(uint64 ID);

private:
	void SaveGameObjectSingle(Config& config, GameObject* gameObject);
	void SaveGameObjectComponent(Config& config, Component* component);

#pragma region Components Save/Load
	void SaveComponent(Component* component, Config&);
	void SaveComponent(C_Camera*, Config&);
	void SaveComponent(C_Animation*, Config&);
	void SaveComponent(C_ParticleSystem*, Config&);

	void LoadComponent(Component* component, Config&);
	void LoadComponent(C_Camera*, Config&);
	void LoadComponent(C_Animation*, Config&);
	void LoadComponent(C_ParticleSystem*, Config&);
#pragma endregion
private:
	bool fbx_loaded = false;
	uint material_count = 0;
	LCG random;

};

namespace Importer
{
	namespace Scenes
	{
		//Processes an already loaded FBX file and loads it into an assimp scene structure
		//Warning: (TODO) do we need to release aiScene data later? (aiReleaseImport is not specificed in Assimp)
		const aiScene* ProcessAssimpScene(const char* buffer, uint size);

		//Processes an already loaded FBX file and generates all the hierarchy and components setup
		//Warning: meshes, materials and lights need to be linked later, this function only loads the hierarchy
		//GameObjects that are meant to have a mesh, material or light, are added the component and given the id from the aiScene container.
		GameObject* Import(const aiScene* scene);

		//Processes an aiNode structure and generates the GameObject hierarchy recursively.
		//Warning: meshes, materials and lights need to be linked later, this function only loads the hierarchy
		//GameObjects that are meant to have a mesh, material or light, are added the component and given the id from the aiScene container.
		GameObject* CreateGameObjectFromNode(const aiScene* scene, const aiNode* node, GameObject* parent);

		//Process a GameObject data with its hierarchy into a buffer file saved as json
		//Returns the size of the buffer file (0 if any errors)
		//Warning: buffer memory needs to be released after the function call
		uint64 SaveScene(const GameObject* root, char** buffer);

		//Process a GameObject data with its hierarchy into a config file
		//This function will be called recursively for every child in <gameObject>
		void SaveGameObject(Config& config, const GameObject* gameObject);

		//Process a Component base data into a config file
		//This function will call specific functions for each component type
		void SaveComponentBase(Config& config, const Component* component);

		//Select the specific component class to be saved and calls its according function
		void SaveComponent(Config& config, const Component* component);

		//Process a Camera component data into a config file
		void SaveComponent(Config& config, const C_Camera* component);

		//Process an Animation component data into a config file
		void SaveComponent(Config& config, const C_Animation* component);

		//Process a Particle System component data into a config file
		void SaveComponent(Config& config, const C_ParticleSystem* component);
	}
}

#endif