#ifndef __M_RESOURCES_H__
#define __M_RESOURCES_H__

#include "Module.h"
#include "Resource.h"
#include <map>
#include <vector>

struct aiMesh;
class R_Mesh;

class M_Resources : public Module
{
public:
	M_Resources(bool start_enabled = true);
	~M_Resources();

	bool Init(Config& config);
	bool CleanUp();

	R_Mesh* ImportRMesh(const aiMesh* from, const char* source_file);

	Resource* GetResource(unsigned long long ID, Resource::Type type);

	void FinishImporting();

private:
	std::map<unsigned long long, Resource*> resources;
	std::vector<Resource*> importingResources;

	unsigned long long nextID = 1;
};

#endif
