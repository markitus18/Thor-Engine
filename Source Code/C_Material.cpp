#include "C_Material.h"
#include "GameObject.h"
#include "OpenGL.h"

C_Material::C_Material(const GameObject* gameObject) : Component (Type::Material, gameObject, true)
{
	
}

C_Material::~C_Material()
{

}

Component::Type C_Material::GetType()
{
	return Component::Type::Material;
}