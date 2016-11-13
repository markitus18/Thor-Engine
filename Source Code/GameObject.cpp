#include "GameObject.h"
#include "OpenGL.h"
#include "Globals.h"
#include "C_Camera.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

GameObject::GameObject()
{
}

GameObject::GameObject(GameObject* new_parent, const char* new_name, const float3& translation, const float3& scale, const Quat& rotation) : name(new_name)
{
	//Hierarchy setup ---
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

void GameObject::Update()
{
	if (transform)
	{
		if (transform->transform_updated)
		{
			OnUpdateTransform();
		}
	}

	for (uint i = 0; i < childs.size(); i++)
	{
		if (childs[i]->active)
			childs[i]->Update();
	}
}

void GameObject::Draw(bool shaded, bool wireframe)
{
	if (active && IsParentActive())
	{
		C_Mesh* mesh = GetComponent<C_Mesh>();
		if (mesh)
		{
			App->renderer3D->AddMesh(transform->GetGlobalTransformT(), mesh, GetComponent<C_Material>(), shaded, wireframe, selected, IsParentSelected(), flipped_normals);
		}

		C_Camera* camera = GetComponent<C_Camera>();
		if (camera)
		{
			App->renderer3D->AddFrustum(camera->frustum, Blue);
		}

		if (selected || IsParentSelected())
		{
			App->renderer3D->AddAABB(aabb, Green);
			App->renderer3D->AddOBB(obb, Yellow);
		}
	}
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

bool GameObject::HasFlippedNormals() const
{
	if (parent)
	{
		return transform->flipped_normals != parent->HasFlippedNormals() ? true : false;
	}
	return transform->flipped_normals;
}

bool GameObject::IsParentActive() const
{
	if (active == false)
		return false;

	if (parent)
		return parent->IsParentActive();

	return active;
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

Component* GameObject::CreateComponent(Component::Type type)
{
	Component* new_component = nullptr;
	switch (type)
	{
		case(Component::Type::Mesh):
		{
			if (!HasComponent(Component::Type::Mesh))
			{
				new_component = new C_Mesh(this);
			}
		}
		case(Component::Type::Material):
		{
			C_Mesh* mesh = GetComponent<C_Mesh>();
			if (mesh)
			{
				new_component = new C_Material(this);
			}
		}
		case(Component::Type::Camera):
		{
			if (!HasComponent(Component::Type::Camera))
			{
				new_component = new C_Camera(this);
			}
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
		aabb.Enclose(transform->GetGlobalPosition() - float3(1, 1, 1), transform->GetGlobalPosition() + float3(1, 1, 1));
		obb = aabb;
	}
}

//void GameObject::GetComponents(Component::Type type, std::vector<Component*>& vec)
//{
//	for (uint i = 0; i < components.size(); i++)
//	{
//		if (components[i]->GetType() == type)
//		{
//			vec.push_back(components[i]);
//		}
//	}
//}