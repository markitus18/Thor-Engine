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

		C_Mesh* LoadMesh(const aiMesh* mesh);
		void	LoadBuffers(C_Mesh* mesh);

	private:
		std::vector<C_Mesh*> meshes;
};

#endif
