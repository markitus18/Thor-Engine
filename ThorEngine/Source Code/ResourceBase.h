#ifndef __RESOURCE_BASE_H__
#define __RESOURCE_BASE_H__

#include <string>
#include <vector>

#include "Config.h"
#include "Globals.h"

enum class ResourceType
{
	FOLDER,
	MESH,
	TEXTURE,
	MATERIAL,
	ANIMATION,
	ANIMATOR_CONTROLLER,
	MODEL,
	PARTICLESYSTEM,
	SHADER,
	MAP,
	UNKNOWN,
};

struct ResourceBase
{
	uint64 ID = 0;
	ResourceType type = ResourceType::UNKNOWN;

	std::string name = "";
	std::string assetsFile = "";
	std::string libraryFile = "";

	std::vector<uint64> containedResources;

	ResourceBase() {}; //Looks like we need default constructor for maps
	ResourceBase(ResourceType type, const char* file, const char* name, uint64 id) : type(type), assetsFile(file), name(name ? name : ""), ID(id) {};

	bool Compare(const char* file, const char* name, ResourceType type) const
	{
		return (this->assetsFile == file && (name ? this->name == name : true) && (type != ResourceType::UNKNOWN ? type == this->type : true));
	}

	void Serialize(Config& config) const
	{
		config.SetNumber("ID", ID);
		config.SetString("Name", name.c_str());
		config.SetNumber("Type", static_cast<int>(type));
		config.SetString("Library file", libraryFile.c_str());
	}
};

#endif //__RESOURCE_BASE_H__
