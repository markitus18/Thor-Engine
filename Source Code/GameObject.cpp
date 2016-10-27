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


void GameObject::Draw(bool shaded, bool wireframe)
{
	if (transform)
	{
		if (transform->transform_updated)
		{
			OnUpdateTransform();
		}
	}

	C_Mesh* mesh = GetComponent<C_Mesh>();
	if (mesh)
	{
		App->renderer3D->AddMesh(transform->GetGlobalTransformT(), mesh, mesh->materials[0], shaded, wireframe, selected, IsParentSelected());
		if (selected || IsParentSelected())
		{
			App->renderer3D->AddAABB(mesh->GetGlobalAABB(), Green);
			App->renderer3D->AddOBB(mesh->GetGlobalOBB(), Yellow);
		}
	}

	C_Camera* camera = GetComponent<C_Camera>();
	if (camera)
	{
		App->renderer3D->AddFrustum(camera->frustum, Blue);
	}

	for (uint i = 0; i < childs.size(); i++)
	{
		if (childs[i]->active)
			childs[i]->Draw(shaded, wireframe);	
	}
}

void GameObject::OnUpdateTransform()
{
	flipped_normals = transform->flipped_normals;
	C_Mesh* mesh = GetComponent<C_Mesh>();
	if (mesh)
	{
		mesh->flipped_normals = HasFlippedNormals();
	}

	//Updating components
	for (uint i = 0; i < components.size(); i++)
	{
		float4x4 global_parent = float4x4::identity;
		if (parent)
		{
			global_parent = parent->GetComponent<C_Transform>()->GetGlobalTransform();
		}
		components[i]->OnUpdateTransform(transform->GetGlobalTransform(), global_parent);
	}

	//Updating childs transform
	for (uint i = 0; i < childs.size(); i++)
	{
		childs[i]->OnUpdateTransform();
	}
}

void GameObject::UpdateCamera()
{
	C_Camera* camera = GetComponent<C_Camera>();
	if (camera)
	{
		float4x4 trans = transform->GetGlobalTransform();
		camera->frustum.SetFront(trans.WorldZ());
		camera->frustum.SetUp(trans.WorldY());

		float3 position = float3::zero;
		float3 scale = float3::one;
		Quat quat = Quat::identity;
		trans.Decompose(position, quat, scale);
		camera->frustum.SetPos(position);
	}
}

bool GameObject::HasFlippedNormals() const
{
	if (parent)
	{
		return flipped_normals != parent->HasFlippedNormals() ? true : false;
	}
	return flipped_normals;
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
				new_component = mesh->CreateMaterial();
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
				((C_Mesh*)component)->flipped_normals = HasFlippedNormals();
			}
			break;
		}
		case(Component::Type::Material):
		{
			C_Mesh* mesh = GetComponent<C_Mesh>();
			if (mesh > 0)
			{
				mesh->AddMaterial((C_Material*)component);
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