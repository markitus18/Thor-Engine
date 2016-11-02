#include "C_Material.h"
#include "GameObject.h"
#include "OpenGL.h"

C_Material::C_Material(const GameObject* gameObject) : Component (Type::Material, gameObject)
{

}

C_Material::~C_Material()
{
	//TODO: Temporal, want to move GL functions to Loaders and GameObject
	glDeleteBuffers(1, &texture_id);
}

void C_Material::Save()
{

}

void C_Material::Load()
{

}

Component::Type C_Material::GetType()
{
	return Component::Type::Material;
}