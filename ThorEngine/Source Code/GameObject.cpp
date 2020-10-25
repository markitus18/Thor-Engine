#include "GameObject.h"
#include "OpenGL.h"
#include "Globals.h"
#include "Engine.h"
#include "M_Renderer3D.h"
#include "Scene.h"

#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_Camera.h"
#include "C_Animator.h"
#include "C_Billboard.h"
#include "C_ParticleSystem.h"

GameObject::GameObject() : TreeNode(GAMEOBJECT)
{
	AddComponent(new C_Transform(this, float3::zero, Quat::identity, float3::one));
}

GameObject::GameObject(GameObject* parent, const char* name, const float3& translation, const Quat& rotation, const float3& scale) : name(name), TreeNode(GAMEOBJECT)
{
	this->parent = parent;
	if (parent)
		parent->childs.push_back(this);

	AddComponent(new C_Transform(this, translation, rotation, scale));
	
}

GameObject::GameObject(GameObject* parent, const float4x4& transform, const char* name) : name(name), TreeNode(GAMEOBJECT)
{
	this->parent = parent;
	if (parent)
		parent->childs.push_back(this);

	AddComponent(new C_Transform(this, transform));
}

GameObject::~GameObject()
{
	for (uint i = 0; i < childs.size(); i++)
	{
		delete childs[i];
		childs[i] = nullptr;
	}
	childs.clear();

	for (uint i = 0; i < components.size(); i++)
	{
		delete components[i];
		components[i] = nullptr;
	}
}

void GameObject::Destroy()
{
	if (parent)
	{
		parent->RemoveChild(this);
		parent = nullptr;
	}
	
	//Notify destroy. Done before children so parents stack first
	if (sceneOwner)
		sceneOwner->OnDestroyGameObject(this);

	//Destroy all children
	for (uint i = 0; i < childs.size(); ++i)
	{
		childs[i]->Destroy();
	}
}

void GameObject::Update(float dt)
{
	//Throw on update transform event if matrix has been updated
	if (transform && transform->transform_updated)
	{
		OnUpdateTransform();
	}

	//Updating all components
	std::vector<Component*>::iterator it;
	for (it = components.begin(); it != components.end(); ++it)
	{
		(*it)->Update(dt);
	}

	//Iterate through all children
	for (uint i = 0; i < childs.size(); i++)
	{
		if (childs[i]->active)
			childs[i]->Update(dt);
	}
}

void GameObject::Draw(RenderingFlags flags)
{
	if (active && IsParentActive())
	{
		std::vector<Component*>::iterator it;
		for (it = components.begin(); it != components.end(); ++it)
		{
			if ((*it)->IsActive())
				(*it)->Draw(flags);
		}

		if (flags & RenderFlag_Bounds_AABB)
			Engine->renderer3D->AddAABB(aabb, Green);
		if (flags & RenderFlag_Bounds_OBB)
			Engine->renderer3D->AddOBB(obb, Yellow);
	}
}

void GameObject::DrawResursive(RenderingFlags flags)
{
	Draw(flags);

	for (uint i = 0; i < childs.size(); i++)
		childs[i]->DrawResursive(flags);
}

void GameObject::OnUpdateTransform()
{
	flipped_normals = HasFlippedNormals();

	//Updating components
	float4x4 global_parent = float4x4::identity;
	if (parent)
	{
		global_parent = parent->GetComponent<C_Transform>()->GetGlobalTransform();
	}
	for (uint i = 0; i < components.size(); i++)
	{
		components[i]->OnUpdateTransform(transform->GetGlobalTransform(), global_parent);
	}

	//Updating childs transform
	for (uint i = 0; i < childs.size(); i++)
	{
		childs[i]->OnUpdateTransform();
	}

	UpdateAABB();
}

const AABB& GameObject::GetAABB() const
{
	return aabb;
}

const OBB& GameObject::GetOBB() const
{
	return obb;
}

bool GameObject::HasFlippedNormals() const
{
	if (parent)
	{
		return transform->flipped_normals != parent->HasFlippedNormals() ? true : false;
	}
	return transform->flipped_normals;
}

void GameObject::SetParent(GameObject* gameObject, GameObject* next, bool worldPositionStays)
{
	if (this != gameObject && gameObject != nullptr && parent != gameObject)
	{
		float4x4 global = transform->GetTransform();
		if (parent != nullptr)
		{
			global = transform->GetGlobalTransform();
			parent->RemoveChild(this);
		}
		parent = gameObject;

		std::vector<GameObject*>::iterator it = next ? std::find(parent->childs.begin(), parent->childs.end(), next) : parent->childs.end();
		parent->childs.insert(it, this);

		transform->SetGlobalTransform(global);
	}
}

bool GameObject::IsParentActive() const
{
	if (active == false)
		return false;

	if (parent)
		return parent->IsParentActive();

	return active;
}

void GameObject::RemoveChild(GameObject* gameObject)
{
	std::vector<GameObject*>::iterator it = childs.begin();
	for (/**/; it != childs.end(); it++)
	{
		if ((*it) == gameObject)
		{
			childs.erase(it);
			break;
		}
	}
}

bool GameObject::HasChildInTree(GameObject* gameObject) const //TODO: Change to iterative function (optimization)
{
	std::vector<GameObject*>::const_iterator it;
	for (it = childs.begin(); it != childs.end(); ++it)
	{
		if (*it == gameObject)
			return true;
	}
	for (it = childs.begin(); it != childs.end(); ++it)
	{
		if ((*it)->HasChildInTree(gameObject))
			return true;
	}
	return false;
}

void GameObject::CollectChilds(std::vector<GameObject*>& vector)
{
	vector.push_back(this);
	for (uint i = 0; i < childs.size(); i++)
		childs[i]->CollectChilds(vector);
}

void GameObject::CollectChilds(std::vector<const GameObject*>& vector) const
{
	vector.push_back(this);
	for (uint i = 0; i < childs.size(); i++)
		childs[i]->CollectChilds(vector);
}

GameObject* GameObject::FindChildByName(const char* name) const
{
	std::vector<GameObject*>::const_iterator it;
	for (it = childs.begin(); it != childs.end(); ++it)
	{
		if ((*it)->name == name)
		{
			return *it;
		}
	}
	return nullptr;
}

GameObject* GameObject::GetChild(uint index) const
{
	if (index < childs.size())
	{
		return childs[index];
	}
	return nullptr;
}

int GameObject::GetChildIndex(GameObject* gameObject) const
{
	int count = 0;
	std::vector<GameObject*>::const_iterator it;
	for (it = childs.begin(); it != childs.end(); ++it)
	{
		if ((*it) == gameObject)
		{
			return count;
		}
		++count;
	}
	return -1;
}

std::vector<TreeNode*> GameObject::GetChilds() const
{
	std::vector<TreeNode*> ret(childs.begin(), childs.end());
	return ret;
}

TreeNode* GameObject::GetParentNode() const
{
	return parent;
}

bool GameObject::IsNodeActive() const
{
	return IsParentActive();
}

bool GameObject::DrawTreeNode() const
{
	return true;
}

void GameObject::SetParentNode(TreeNode* parent, TreeNode* next)
{
	SetParent((GameObject*)parent, (GameObject*)next);
}

const char* GameObject::GetName() const
{
	return name.c_str();
}

unsigned long long GameObject::GetID() const
{
	return uid;
}

void GameObject::SetStatic(bool isStatic, bool setChildren)
{
	if (this->isStatic != isStatic)
	{
		this->isStatic = isStatic;

		//Iterate up through all parents since they must also be static
		GameObject* it = parent;
		while (it != nullptr && it->uid != 0)
		{
			it->SetStatic(isStatic, false);
			it = it->parent;
		}

		//Iterate down the hierarchy tree
		if (setChildren)
			for (uint i = 0; i < childs.size(); ++i)
				childs[i]->SetStatic(isStatic, setChildren);

		if (sceneOwner)
			sceneOwner->OnGameObjectStaticChanged(this);
	}
}

Component* GameObject::CreateComponent(Component::Type type)
{
	Component* new_component = nullptr;
	switch (type)
	{
		case(Component::Type::Mesh):
		{
			if (!HasComponent(Component::Mesh))
				new_component = new C_Mesh(this);
			break;
		}
		case(Component::Type::Material):
		{
			if (!HasComponent(Component::Material))
				new_component = new C_Material(this);
			break;
		}
		case(Component::Type::Camera):
		{
			if (!HasComponent(Component::Material))
			{
				new_component = new C_Camera(this);
				new_component->OnUpdateTransform(transform->GetGlobalTransform(), float4x4::identity);
			}
			break;
		}
		case(Component::Type::Animator):
		{
			if (!HasComponent(Component::Animator))
				new_component = new C_Animator(this);
			break;
		}
		case(Component::Type::Billboard):
		{
			if (!HasComponent(Component::Billboard))
				new_component = new C_Billboard(this);
		}
		case(Component::Type::ParticleSystem):
		{
			if (!HasComponent(Component::ParticleSystem))
				new_component = new C_ParticleSystem(this);
		}
	}
	if (new_component)
	{
		components.push_back(new_component);
	}
	return new_component;
}

void GameObject::AddComponent(Component* component)
{
	//Check if single component types already exist
	switch (component->GetType())
	{
		case(Component::Type::Transform):
		{
			if (!HasComponent(Component::Type::Transform))
			{
				components.push_back(component);
				transform = (C_Transform*)component;
				component->gameObject = this;
				OnUpdateTransform();
			}
			break;
		}
		case(Component::Type::Mesh):
		{
			if (!HasComponent(Component::Type::Mesh))
			{
				components.push_back(component);
				component->gameObject = this;
				component->OnUpdateTransform(GetComponent<C_Transform>()->GetGlobalTransform());
				UpdateAABB();
			}
			break;
		}
		case(Component::Type::Material):
		{
			if (HasComponent(Component::Type::Mesh) && !HasComponent(Component::Type::Material))
			{
				components.push_back(component);
				component->gameObject = this;
			}

			break;
		}
		case(Component::Type::Camera):
		{
			components.push_back((Component*)component);
			component->gameObject = this;
			break;
		}
	}
}

bool GameObject::HasComponent(Component::Type type)
{
	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type)
			return true;
	}
	return false;
}

void GameObject::UpdateAABB()
{
	C_Mesh* mesh = GetComponent <C_Mesh>();
	if (mesh)
	{
		AABB meshAABB = mesh->GetAABB();
		obb = mesh->GetAABB();
		obb.Transform(GetComponent< C_Transform>()->GetGlobalTransform());

		aabb.SetNegativeInfinity();
		aabb.Enclose(obb);
	}
	else
	{
		aabb.SetNegativeInfinity();
		aabb.SetFromCenterAndSize(transform->GetGlobalPosition(), float3(1, 1, 1));
		obb = aabb;
	}
}