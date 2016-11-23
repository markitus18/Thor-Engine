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

		//FBX conversion
		C_Mesh* ImportMesh(const aiMesh* mesh, const char* file);

		//Own file format conversion
		void	SaveMesh(const C_Mesh& mesh, const char* path);
		C_Mesh* LoadMesh(const char* path);

		R_Mesh*	ImportMeshResource(const aiMesh* mesh);
		void	SaveMeshResource(const R_Mesh*, const char* path);

	private:
		std::vector<C_Mesh*> meshes;
};

#endif
