#include "M_Resources.h"
#include "Application.h"

//Loaders
#include "M_Meshes.h"
#include "M_Materials.h"

//Resources
#include "R_Mesh.h"
#include "R_Texture.h"
#include "R_Material.h"


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

R_Mesh* M_Resources::ImportRMesh(const aiMesh* mesh, const char* source_file, const char* name)
{
	R_Mesh* resource = nullptr;

	//First we find if that resource has already been imported
	resource = (R_Mesh*)FindResourceInLibrary(source_file, name, Resource::MESH);
	if (resource != nullptr)
		return resource;

	//If its not imported, import it
	resource = App->moduleMeshes->ImportMeshResource(mesh, ++nextID, source_file, name);
	if (resource)
	{
		existingResources[resource->ID] = GetMetaInfo(resource);
		importingResources.push_back(resource);
	}

	return resource;
}

R_Texture* M_Resources::ImportRTexture(const char* buffer, const char* source_file, uint size)
{
	R_Texture* resource = nullptr;

	//First we find if that resource has already been imported
	resource = (R_Texture*)FindResourceInLibrary(source_file, "", Resource::TEXTURE);
	if (resource != nullptr)
		return resource;

	//If its not imported, import it
	resource = App->moduleMaterials->ImportTextureResource(buffer, ++nextID, source_file, size);
	if (resource)
	{
		existingResources[resource->ID] = GetMetaInfo(resource);
		importingResources.push_back(resource);
	}
	return resource;
}

R_Material* M_Resources::ImportRMaterial(const aiMaterial* mat, const char* source_file, const char* name)
{
	R_Material* resource = nullptr;

	//First we find if that resource has already been imported
	resource = (R_Material*)FindResourceInLibrary(source_file, name, Resource::MATERIAL);
	if (resource != nullptr)
		return resource;

	//If its not imported, import it
	resource = App->moduleMaterials->ImportMaterialResource(mat, ++nextID, source_file);
	if (resource)
	{
		existingResources[resource->ID] = GetMetaInfo(resource);
		importingResources.push_back(resource);
	}
	return resource;
}

Resource* M_Resources::GetResource(uint64 ID, Resource::Type type)
{
	Resource* ret = nullptr;
	//First find if the wanted resource is loaded
	std::map<uint64, Resource*>::iterator it = resources.find(ID);
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
				ret = App->moduleMaterials->LoadTextureResource(ID);
				break;
			}
			case (Resource::MATERIAL):
			{
				ret = App->moduleMaterials->LoadMaterialResource(ID);
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

Resource* M_Resources::FindResourceInLibrary(const char* original_file, const char* name, Resource::Type type)
{
	for (std::map<uint64, ResourceMeta>::iterator it = existingResources.begin(); it != existingResources.end(); it++)
	{
		if (it->second.Compare(original_file, name))
		{
			return GetResource(it->first, type);
		}
	}
	return nullptr;
}

ResourceMeta M_Resources::GetMetaInfo(Resource* resource)
{
	ResourceMeta ret;

	ret.original_file = resource->original_file;
	ret.resource_name = resource->name;
	ret.type = resource->type;

	return ret;
}