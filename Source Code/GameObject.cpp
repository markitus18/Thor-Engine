#include "GameObject.h"

GameObject::GameObject()
{
}
GameObject::GameObject(const GameObject* parent, const char* name) : position(float3::zero), scale(float3::one), rotation(Quat::identity), name(name)
{

}

GameObject::GameObject(const GameObject* new_parent, const float3& translation, const float3& scale, const Quat& rotation, const char* name) : position(translation), scale(scale), rotation(rotation), name(name)
{
	parent = new_parent;
}

GameObject::~GameObject()
{

}


void GameObject::Draw()
{
	if (mesh)
	{
		mesh->Draw();
	}

	for (uint i = 0; i < childs.size(); i++)
	{
		childs[i]->Draw();
	}
}

const float3 GameObject::GetPosition()
{
	return position;
}

const float3 GameObject::GetScale()
{
	return scale;
}

const Quat GameObject::GetQuatRotation()
{
	return rotation;
}

void GameObject::Select()
{
	selected = true;
}

void GameObject::Unselect()
{
	selected = false;
}

bool GameObject::IsSelected()
{
	return selected;
}