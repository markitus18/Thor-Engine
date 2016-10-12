#ifndef __MODULE_IMPORT_H__
#define __MODULE_IMPORT_H__

#include "Module.h"
#include "GameObject.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
class C_Mesh;
class C_Material;

class ModuleImport : public Module
{
public:

	ModuleImport(Application* app, bool start_enabled = true);
	~ModuleImport();

	GameObject* LoadFBX(const aiScene* scene, const aiNode* node, GameObject* parent, char* path);
	uint LoadIMG(const char* path);
	bool Init();
	bool CleanUp();
	update_status Update(float dt);

	void LoadMesh(C_Mesh* mesh, const aiMesh* from);
	//Tmp function, move to file system
	void CutPath(std::string&);
	std::string GetFileFolder(const std::string&);
private:
	bool fbx_loaded = false;
	uint material_count = 0;

};

#endif