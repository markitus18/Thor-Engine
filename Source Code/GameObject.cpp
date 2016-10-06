#include "GameObject.h"

GameObject::GameObject()
{
}
GameObject::GameObject(const GameObject* parent) : position(float3::zero), scale(float3::one), rotation(Quat::identity)
{

}

GameObject::GameObject(const GameObject* new_parent, const float3& translation, const float3& scale, const Quat& rotation) : position(translation), scale(scale), rotation(rotation)
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