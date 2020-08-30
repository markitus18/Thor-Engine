#include "C_Material.h"
#include "GameObject.h"
#include "OpenGL.h"

#include "R_Material.h"

C_Material::C_Material(GameObject* gameObject) : Component (Type::Material, gameObject, true)
{
	
}

C_Material::~C_Material()
{

}

void C_Material::SetResource(Resource* resource)
{
	rMaterialHandle.Set((R_Material*)resource);
}

void C_Material::SetResource(unsigned long long id)
{
	rMaterialHandle.Set(id);
}

uint64 C_Material::GetResourceID() const
{
	return rMaterialHandle.GetID();
}