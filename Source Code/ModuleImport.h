#ifndef __MODULE_IMPORT_H__
#define __MODULE_IMPORT_H__

#include "Module.h"
#include "GameObject.h"

class ModuleImport : public Module
{
public:

	ModuleImport(Application* app, bool start_enabled = true);
	~ModuleImport();

	void LoadFBX(char* path);

	bool Init();
	bool CleanUp();
	update_status Update(float dt);

	Mesh* GetMesh();
public:
	GameObject robotMesh;

private:

};

#endif