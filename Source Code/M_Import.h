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

class Config;

class M_Import : public Module
{
public:

	M_Import(Application* app, bool start_enabled = true);
	~M_Import();

	void ImportFile(char* path);
	GameObject* LoadFBX(const aiScene* scene, const aiNode* node, GameObject* parent, char* path, std::vector<GameObject*>& vector);
	bool Init(Config& config);
	bool CleanUp();
	update_status Update(float dt);

	void SaveGameObjectConfig(Config& config, std::vector<GameObject*>& gameObjects);
	GameObject* LoadGameObject(const char* path);
	void LoadGameObjectConfig(Config& config, std::vector<GameObject*>& nodes);

private:
	void SaveGameObjectSingle(Config& config, GameObject* gameObject);
	void FindGameObjectByID(uint id, GameObject* gameObject, GameObject** ret);

private:
	bool fbx_loaded = false;
	uint material_count = 0;
	LCG random;

};

#endif