#ifndef __MODULE_MESHES_H__
#define __MODULE_MESHES_H__

#include "Module.h"
#include "R_Mesh.h"
#include <vector>

class C_Mesh;
class R_Mesh;

struct aiMesh;

class M_Meshes : public Module
{
	public:
		M_Meshes(bool start_enabled = true);
		~M_Meshes();

		bool Init();
		bool CleanUp();


		R_Mesh*	ImportMeshResource(const aiMesh* mesh, unsigned long long ID, const char* file, const char* name);

		bool	SaveMeshResource(const R_Mesh*);
		R_Mesh* LoadMeshResource(u64 ID);

	private:
		std::vector<C_Mesh*> meshes;
};

#endif
