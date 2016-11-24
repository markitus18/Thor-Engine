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
		it->second->FreeMemory();
		RELEASE(it->second);
		it = resources.erase(it);
	}

	return true;
}

R_Mesh* M_Resources::ImportRMesh(const aiMesh* mesh, const char* source_file)
{
	unsigned long long ret;
	R_Mesh* resource = App->moduleMeshes->ImportMeshResource(mesh, nextID, source_file);
	if (resource)
	{
		importingResources.push_back(resource);
		ret = nextID;
		nextID++;
	}

	return resource;
}

Resource* M_Resources::GetResource(unsigned long long ID, Resource::Type type)
{
	Resource* ret = nullptr;
	//First find if the wanted resource is loaded
	std::map<unsigned long long, Resource*>::iterator it = resources.find(ID);
	if (it != resources.end())
	{
		ret = it->second;
		LOG("Getting resource already loaded");
	}
	else
	{
		
		//If resource is not loaded, search in library
		switch (type)
		{
			case (Resource::MESH):
			{
				ret = App->moduleMeshes->LoadMeshResource(ID);
				break;
			}
			case (Resource::TEXTURE):
			{
				break;
			}
			case (Resource::BONE):
			{
				break;
			}
		}

		if (ret != nullptr)
		{
			ret->LoadOnMemory();
			LOG("Resource load from library correctly");
		}
	}
	return ret;
}

void M_Resources::FinishImporting()
{
	for (std::vector<Resource*>::iterator it = importingResources.begin(); it != importingResources.end(); /**/)
	{
		RELEASE(*it);
		it = importingResources.erase(it);
	}
}