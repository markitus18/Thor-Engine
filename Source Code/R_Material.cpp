#include "R_Material.h"

//TODO: temporal, just to get default shader
#include "Engine.h"
#include "R_Shader.h"
#include "M_Resources.h"

R_Material::R_Material() : Resource(ResourceType::MATERIAL)
{
	isInternal = true;
	hShader.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Shaders/Default Shader_PlainLight.shader")->ID);
}

R_Material::~R_Material()
{

}
