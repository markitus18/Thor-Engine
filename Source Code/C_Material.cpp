#include "C_Material.h"
#include "GameObject.h"
#include "OpenGL.h"

C_Material::C_Material(GameObject* gameObject) : Component (Type::Material, gameObject, true)
{
	
}

C_Material::~C_Material()
{

}