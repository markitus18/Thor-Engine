#include "I_Folders.h"

#include "Config.h"
#include "R_Folder.h"

R_Folder* Importer::Folders::Create()
{
	return new R_Folder();
}

uint64 Importer::Folders::Save(const R_Folder* rFolder, char** buffer)
{
	Config file;
	Config_Array resourcesNode = file.SetArray("Containing Resources");

	for (uint i = 0; i < rFolder->baseData->containedResources.size(); ++i)
		resourcesNode.AddNumber(rFolder->baseData->containedResources[i]);

	return file.Serialize(buffer);
}

void Importer::Folders::Load(const char* buffer, R_Folder* rFolder)
{
	Config file(buffer);
	Config_Array resourcesNode = file.GetArray("Containing Resources");

	for (uint i = 0; i < resourcesNode.GetSize(); ++i)
		rFolder->AddContainedResource(resourcesNode.GetNumber(i));
}