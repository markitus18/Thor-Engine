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

	R_Prefab* ImportFile(const char* path, Uint32 ID);
	GameObject* CreateGameObjects(const aiScene*, const aiNode*, GameObject*, const char*, std::vector<GameObject*>&, std::vector<const aiMesh*>&, std::vector<const GameObject*>&);
	bool Init(Config& config);
	bool CleanUp();
	update_status Update(float dt);

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

#endif