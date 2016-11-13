#ifndef __MODULE_MESHES_H__
#define __MODULE_MESHES_H__

#include "Module.h"
#include <vector>

struct C_Mesh;
struct aiMesh;

class ModuleMeshes : public Module
{
	public:
		ModuleMeshes(Application* app, bool start_enabled = true);
		~ModuleMeshes();

		bool Init();
		bool CleanUp();

		//FBX conversion
		C_Mesh* ImportMesh(const aiMesh* mesh, const char* file);

		//Own file format conversion
		void	SaveMesh(const C_Mesh& mesh, const char* path);
		C_Mesh* LoadMesh(const char* path);

	private:
		std::vector<C_Mesh*> meshes;
};

#endif
