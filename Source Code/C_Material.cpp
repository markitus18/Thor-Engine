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

void C_Material::StackTexture()
{
	//TODO: Temporal, want to move GL functions to Loaders and GameObject
	if (texture_id)
	{
		glBindTexture(GL_TEXTURE_2D, texture_id);
	}
	glColor4f(color.r, color.g, color.b, color.a);

}

void C_Material::PopTexture()
{
	//TODO: Temporal, want to move GL functions to Loaders and GameObject
	glColor4f(255, 255, 255, 1.0);
	if (texture_id)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

Component::Type C_Material::GetType()
{
	return Component::Type::Material;
}