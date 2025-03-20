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

void GameObject::Update()
{
	//Throw on update transform event if matrix has been updated
	if (transform && transform->transform_updated)
	{
		OnTransformUpdated();
	}

	//Updating all components
	std::vector<Component*>::iterator it;
	for (it = components.begin(); it != components.end(); ++it)
	{
		(*it)->Update();
	}

	//Iterate through all children
	for (uint i = 0; i < childs.size(); i++)
	{
		if (childs[i]->active)
			childs[i]->Update();
	}
}

void GameObject::Draw(ERenderingFlags::Flags flags)
{
	if (active && IsParentActive())
	{
		std::vector<Component*>::iterator it;
		for (it = components.begin(); it != components.end(); ++it)
		{
			if ((*it)->IsActive())
				(*it)->Draw(flags);
		}

		if (flags & ERenderingFlags::Bounds_AABB)
			Engine->renderer3D->AddAABB(aabb, Green);
		if (flags & ERenderingFlags::Bounds_OBB)
			Engine->renderer3D->AddOBB(obb, Yellow);
	}
}

void GameObject::DrawResursive(ERenderingFlags::Flags flags)
{
	Draw(flags);

	for (uint i = 0; i < childs.size(); i++)
		childs[i]->DrawResursive(flags);
}

void GameObject::OnTransformUpdated()
{
	//Updating components
	for (uint i = 0; i < components.size(); i++)
	{
		components[i]->OnTransformUpdated();
	}

	//Updating children transform
	for (uint i = 0; i < childs.size(); i++)
	{
		childs[i]->OnTransformUpdated();
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

void GameObject::Serialize(Config& config)
{
	config.Serialize("UID", uid);
	config.Serialize("Name", name, "[Name not found on serialization]");

	uint64 parentUID = parent ? parent->uid : 0;
	config.Serialize("Parent UID", parentUID);

	config.Serialize("Active", active);
	config.Serialize("Static", isStatic);

	config.Serialize("Selected", selected);
	config.Serialize("OpenInHierarchy", hierarchyOpen);

	beenSelected = hierarchyOpen;

	if (config.isSaving)
	{
		Config_Array compConfig = config.SetArray("Components");
		for (uint i = 0; i < components.size(); ++i)
			components[i]->Serialize(compConfig.AddNode());
	}
	else
	{
		Config_Array compConfig = config.GetArray("Components");
		for (uint i = 0; i < compConfig.GetSize(); ++i)
		{
			int componentType = 0;
			compConfig.GetNode(i).Serialize("Component Type", componentType);

			if (Component* component = CreateComponent((Component::Type)componentType))
				component->Serialize(compConfig.GetNode(i));
		}
		OnTransformUpdated();
	}
}

void GameObject::SetParent(GameObject* gameObject, GameObject* next, bool worldPositionStays)
{
	if (this != gameObject && gameObject != nullptr && parent != gameObject)
	{
		if (parent != nullptr)
		{
			parent->RemoveChild(this);
		}
		parent = gameObject;

		std::vector<GameObject*>::iterator it = next ? std::find(parent->childs.begin(), parent->childs.end(), next) : parent->childs.end();
		parent->childs.insert(it, this);

		// Updates the local transform maintaining the global
		transform->SetGlobalTransform(transform->GetTransform());
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
		case(Component::Type::Transform):
		{
			return transform; //No need to create transform, useful to return it for serialization methods
		}
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
				new_component->OnTransformUpdated();
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
			break;
		}
		case(Component::Type::ParticleSystem):
		{
			if (!HasComponent(Component::ParticleSystem))
				new_component = new C_ParticleSystem(this);
			break;
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
			}
			break;
		}
		case(Component::Type::Mesh):
		{
			if (!HasComponent(Component::Type::Mesh))
			{
				components.push_back(component);
				component->gameObject = this;
				component->OnTransformUpdated();
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
	C_Mesh* mesh = GetComponent<C_Mesh>();
	if (mesh)
	{
		AABB meshAABB = mesh->GetAABB();
		obb = mesh->GetAABB();
		obb.Transform(GetComponent< C_Transform>()->GetTransform());

		aabb.SetNegativeInfinity();
		aabb.Enclose(obb);
	}
	else
	{
		aabb.SetNegativeInfinity();
		aabb.SetFromCenterAndSize(transform->GetPosition(), float3(1, 1, 1));
		obb = aabb;
	}
}