#ifndef __MODULE_MATERIALS_H__
#define __MODULE_MATERIALS_H__

#include <list>
#include "Globals.h"
#include "Module.h"
#include "Color.h"

class C_Material;

class ModuleMaterials : public Module
{
public:
	ModuleMaterials(Application* app, bool start_enabled = true);
	~ModuleMaterials();

	bool Init();
	bool CleanUp();

	C_Material* Exists(const char* texture_path) const;
	C_Material* LoadMaterial(const std::string& texture_path, const std::string& file, const Color& color);
	uint LoadIMG(const char* path);

private:
	std::list<C_Material*> materials;
	uint materials_count = 0;
};

#endif // __MODULE_MATERIALS_H__
