#include "I_Folders.h"

#include "Config.h"
#include "R_Folder.h"

uint64 Importer::Folders::Save(const R_Folder* rFolder, char** buffer)
{
	Config file;
	Config_Array resourcesNode = file.SetArray("Containing Resources");

	for (uint i = 0; i < rFolder->containingResources.size(); ++i)
		resourcesNode.AddNumber(rFolder->containingResources[i]);

	return file.Serialize(buffer);
}

void Importer::Folders::Load(const char* buffer, R_Folder* rFolder)
{
	Config file(buffer);
	Config_Array resourcesNode = file.GetArray("Containing Resources");

	for (uint i = 0; i < resourcesNode.GetSize(); ++i)
		rFolder->AddResource(resourcesNode.GetNumber(i));
}