#include "C_Material.h"
#include "GameObject.h"
#include "OpenGL.h"

C_Material::C_Material(GameObject* gameObject) : Component (Type::Material, gameObject)
{

}

C_Material::~C_Material()
{

}

void C_Material::StackTexture()
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

void C_Material::PopTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}