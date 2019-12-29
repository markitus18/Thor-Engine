#include "M_Shaders.h"

#include "R_Shader.h"

void Importer::Shaders::Import(const char* buffer, R_Shader* resShader)
{
	//TODO: in this case importing happens in the shader resource, should we adapt it to other resources?
	resShader->LoadFromText(buffer);
	resShader->Link();
}

uint64 Importer::Shaders::Save(const R_Shader* resShader, char** buffer)
{
	return resShader->Save(buffer);
}

void Importer::Shaders::Load(const char* buffer, uint size, R_Shader* shader)
{
	shader->LoadFromText(buffer);
	//Once we get binary shader working it should be loaded through a binary file
	//shader->LoadFromBinary(buffer, size);
	shader->Link();
}