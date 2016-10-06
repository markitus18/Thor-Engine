#ifndef __MODULE_IMPORT_H__
#define __MODULE_IMPORT_H__

#include "Module.h"
#include "GameObject.h"

struct aiScene;
struct aiNode;

class ModuleImport : public Module
{
public:

	ModuleImport(Application* app, bool start_enabled = true);
	~ModuleImport();

	GameObject* LoadFBX(const aiScene* scene, const aiNode* node, GameObject* parent, char* path);
	uint LoadIMG(char* path);
	bool Init();
	bool CleanUp();
	update_status Update(float dt);

	bool fbx_loaded = false;

};

#endif