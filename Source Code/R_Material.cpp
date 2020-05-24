#include "R_Material.h"

//TODO: temporal, just to get default shader
#include "Engine.h"
#include "M_Resources.h"

R_Material::R_Material() : Resource(Resource::MATERIAL)
{
	//shaderID = Engine->moduleResources->GetResourceInfo("Engine/Assets/Defaults/Default Shader.shader").ID;
	shaderID = Engine->moduleResources->GetResourceInfo("Engine/Assets/Shaders/Default Shader_PlainLight.shader").ID;

	isInternal = true;
}

R_Material::~R_Material()
{

}
