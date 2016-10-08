#include "GameObject.h"
#include "OpenGL.h"

GameObject::GameObject()
{
}
GameObject::GameObject(const GameObject* parent, const char* new_name) : position(float3::zero), scale(float3::one), rotation(Quat::identity), name(new_name)
{
	UpdateTransformMatrix();
}

GameObject::GameObject(const GameObject* new_parent, const float3& translation, const float3& scale, const Quat& rotation, const char* new_name) : position(translation), scale(scale),
						rotation(rotation), name(new_name)
{
	parent = new_parent;
	UpdateTransformMatrix();
}

GameObject::~GameObject()
{

}


void GameObject::Draw()
{
	glPushMatrix();
	glMultMatrixf((float*)&transform);
	if (mesh)
	{
		mesh->Draw();
	}

	for (uint i = 0; i < childs.size(); i++)
	{
		childs[i]->Draw();
	}
	glPopMatrix();
}

float3 GameObject::GetPosition() const
{
	return position;
}

float3 GameObject::GetScale() const
{
	return scale;
}

Quat GameObject::GetQuatRotation() const
{
	return rotation;
}

void GameObject::SetPosition(float3 new_position)
{
	position = new_position;
}

void GameObject::SetScale(float3 new_scale)
{
	scale = new_scale;
}

void GameObject::SetRotation(float3 euler_angles)
{

}

void GameObject::UpdateTransformMatrix()
{
	transform = float4x4::FromTRS(position, rotation, scale);
}

void GameObject::Select()
{
	selected = true;
}

void GameObject::Unselect()
{
	selected = false;
}

bool GameObject::IsSelected() const
{
	return selected;
}

bool GameObject::IsParentSelected() const
{
	if (parent)
	{
		return parent->IsSelected() ? true : parent->IsParentSelected();
	}
	return false;
}
void GameObject::AddMesh(Mesh* new_mesh)
{
	mesh = new_mesh;
	mesh->gameObject = this;
}