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
	return true;
}

unsigned long long M_Resources::ImportRMesh(aiMesh* mesh)
{
	R_Mesh* resource = App->moduleMeshes->ImportMeshResource(mesh);
	resource->ID = nextID;
	std::string path(RequestNewPathID(Resource::Type::MESH));

	if (path != "")
	{
		App->moduleMeshes->SaveMeshResource(resource, path.c_str());
	}
	return path != "" ? resource->ID : 0;
}

std::string M_Resources::RequestNewPathID(Resource::Type type)
{
	std::string path = "";
	switch (type)
	{
		case(Resource::Type::MESH):
		{
			path.append("/Library/Meshes/").append(std::to_string(nextID++)).append(".meshResource");
			break;
		}
		case (Resource::Type::TEXTURE):
		{
			path.append("/Library/Textures/").append(std::to_string(nextID++)).append(".textureResource");
			break;
		}
		case (Resource::Type::BONE):
		{
			path.append("/Library/Bones/").append(std::to_string(nextID++)).append(".boneResource");
			break;
		}
	}
	return path;
}