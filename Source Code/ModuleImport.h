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

class ModuleImport : public Module
{
public:

	ModuleImport(Application* app, bool start_enabled = true);
	~ModuleImport();

	void ImportFile(char* global_path, char* local_destination);
	GameObject* LoadFBX(const aiScene* scene, const aiNode* node, GameObject* parent, char* global_path, char* local_path);
	bool Init(Config& config);
	bool CleanUp();
	update_status Update(float dt);

	//Tmp function, move to file system
	void CutPath(std::string&);
	std::string GetFileFolder(const std::string&);
private:
	bool fbx_loaded = false;
	uint material_count = 0;
	LCG random;

};

#endif