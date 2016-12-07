#include "C_Mesh.h"
#include "OpenGL.h"
#include "GameObject.h"
#include "R_Mesh.h"
#include "Application.h"
#include "M_Resources.h"

C_Mesh::C_Mesh() : Component(Type::Mesh, nullptr, true)
{
}

C_Mesh::C_Mesh(GameObject* new_GameObject) : Component(Type::Mesh, new_GameObject, true)
{
}

C_Mesh::~C_Mesh()
{

}

const AABB& C_Mesh::GetAABB() const
{
	return ((R_Mesh*)App->moduleResources->GetResource(resourceID))->aabb;
}

Component::Type C_Mesh::GetType()
{
	return Component::Type::Mesh;
}

void C_Mesh::Save()
{

}

void C_Mesh::Load()
{

}