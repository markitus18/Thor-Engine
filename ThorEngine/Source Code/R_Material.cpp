#include "R_Material.h"

//TODO: temporal, just to get default shader
#include "Engine.h"
#include "R_Shader.h"
#include "M_Resources.h"

R_Material::R_Material() : Resource(ResourceType::MATERIAL)
{

}

R_Material::~R_Material()
{

}

void R_Material::Serialize(Config& config)
{
	// TODO: Implement Color serialize
	config.Serialize("ColorR", color.r);
	config.Serialize("ColorG", color.g);
	config.Serialize("ColorB", color.b);
	config.Serialize("ColorA", color.a);

	hShader.Serialize("Shader", config);
	hTexture.Serialize("Texture", config);
}
