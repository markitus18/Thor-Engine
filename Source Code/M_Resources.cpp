#include "M_Resources.h"
#include "Application.h"

//Loaders
#include "M_Meshes.h"

//Resources
#include "R_Mesh.h"

M_Resources::M_Resources(bool start_enabled) : Module("Resources", start_enabled)
{

}

M_Resources::~M_Resources()
{

}

bool M_Resources::Init(Config& config)
{
	return true;
}

bool M_Resources::CleanUp()
{
	for (std::map<unsigned long long, Resource*>::iterator it = resources.begin(); it != resources.end(); )
	{
		RELEASE(it->second);
		it = resources.erase(it);
	}

	return true;
}

R_Mesh* M_Resources::ImportRMesh(const aiMesh* mesh)
{
	unsigned long long ret;
	R_Mesh* resource = App->moduleMeshes->ImportMeshResource(mesh, nextID);
	if (resource)
	{
		resources[nextID] = resource;
		ret = nextID;
		nextID++;
	}

	return resource;
}