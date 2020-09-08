#include "I_Shaders.h"

#include "R_Shader.h"

R_Shader* Importer::Shaders::Create()
{
	return new R_Shader();
}

void Importer::Shaders::Import(const char* buffer, R_Shader* resShader)
{
	resShader->LoadFromText(buffer);
	resShader->Link();
}

uint64 Importer::Shaders::Save(const R_Shader* resShader, char** buffer)
{
	return resShader->Save(buffer);
}

void Importer::Shaders::Load(const char* buffer, uint size, R_Shader* shader)
{
	shader->LoadFromBinary(buffer, size);
}