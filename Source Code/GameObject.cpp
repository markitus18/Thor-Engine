#include "GameObject.h"
#include "OpenGL.h"
#include "Globals.h"
#include "Application.h"
#include "M_Renderer3D.h"

GameObject::GameObject()
{
}

GameObject::GameObject(GameObject* new_parent, const char* new_name, const float3& translation, const float3& scale, const Quat& rotation) : name(new_name)
{
	parent = new_parent;
	if (new_parent)
		new_parent->childs.push_back(this);

	AddComponent(new C_Transform(this, translation, rotation, scale));
	
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

void GameObject::Update(float dt)
{
	if (transform)
	{
		if (transform->transform_updated)
		{
			OnUpdateTransform();
		}
	}
	C_Animation* anim = (C_Animation*)GetComponent<C_Animation>();
	if (anim != nullptr)
	{
		anim->Update(dt);
	}

	for (uint i = 0; i < childs.size(); i++)
	{
		if (childs[i]->active)
			childs[i]->Update(dt);
	}
}

void GameObject::Draw(bool shaded, bool wireframe, bool drawBox, bool drawBoxSelected)
{
	if (active && IsParentActive())
	{
		C_Mesh* mesh = GetComponent<C_Mesh>();
		if (mesh)
		{
			App->renderer3D->AddMesh(transform->GetGlobalTransformT(), mesh, GetComponent<C_Material>(), shaded, wireframe, selected, IsParentSelected(), flipped_normals);
		}

		const C_Camera* camera = GetComponent<C_Camera>();
		if (camera)
		{
			App->renderer3D->AddFrustum(camera->frustum, Blue);
		}

		if (drawBox || ( drawBoxSelected && (selected || IsParentSelected())))
		{
			App->renderer3D->AddAABB(aabb, Green);
			App->renderer3D->AddOBB(obb, Yellow);
		}

		const C_Animation* animation = GetComponent<C_Animation>();
		if (animation)
			animation->DrawLinkedBones();
	}
}

void GameObject::DrawResursive(bool shaded, bool wireframe, bool drawBox, bool drawBoxSelected)
{
	Draw(shaded, wireframe, drawBox, drawBoxSelected);

	for (uint i = 0; i < childs.size(); i++)
		childs[i]->DrawResursive(shaded, wireframe, drawBox, drawBoxSelected);
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

void GameObject::SetParent(GameObject* gameObject, bool worldPositionStays)
{
	float4x4 global = transform->GetTransform();
	if (parent != nullptr)
	{
		global = transform->GetGlobalTransform();
		parent->RemoveChild(this);
	}
	parent = gameObject;
	transform->SetGlobalTransform(global);

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

void GameObject::CollectChilds(std::vector<GameObject*>& vector)
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

{	int count = 0;
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

void GameObject::SetStatic(bool isStatic)
{
	this->isStatic = isStatic;
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
		case(Component::Type::Animation):
		{
			if (!HasComponent(Component::Material))
				new_component = new C_Animation(this);
			break;
		}
		case(Component::Type::Bone):
		{
			if (!HasComponent(Component::Material))
				new_component = new C_Bone(this);
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