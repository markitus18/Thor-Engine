#include "GameObject.h"
#include "OpenGL.h"
#include "Globals.h"

GameObject::GameObject()
{
}

GameObject::GameObject(GameObject* new_parent, const char* new_name, const float3& translation, const float3& scale, const Quat& rotation) : name(new_name), position(translation), scale(scale),
						rotation(rotation)
{
	//Hierarchy setup ---
	parent = new_parent;
	if (new_parent)
		new_parent->childs.push_back(this);

	//Matrix setup
	UpdateTransformMatrix();
	UpdateEulerAngles();
}

GameObject::~GameObject()
{
	for (uint i = 0; i < childs.size(); i++)
	{
		delete childs[i];
		childs[i] = NULL;
	}
	childs.clear();

	for (uint i = 0; i < components.size(); i++)
	{
		delete components[i];
		components[i] = NULL;
	}
}


void GameObject::Draw(bool shaded, bool wireframe)
{
	C_Mesh* _mesh = NULL;
	for(uint i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == Component::Type::Mesh)
		{
			_mesh = (C_Mesh*)components[i];
		}
	}
	if (_mesh)
	{
		glPushMatrix();
		glMultMatrixf((float*)&global_transformT);
		_mesh->Draw(shaded, wireframe);
		glPopMatrix();
		if (selected || IsParentSelected())
			DrawAABB();
	}

	for (uint i = 0; i < childs.size(); i++)
	{
		if (childs[i]->active)
			childs[i]->Draw(shaded, wireframe);	
	}
}

void GameObject::DrawAABB()
{
	glDisable(GL_LIGHTING);
	int num_v_aabb = global_AABB.NumVerticesInEdgeList();
	vec* vertices_aabb = new vec[num_v_aabb];
	global_AABB.ToEdgeList((vec*)vertices_aabb);

	int num_v_obb = global_OBB.NumVerticesInEdgeList();
	vec* vertices_obb = new vec[num_v_obb];
	global_OBB.ToEdgeList((vec*)vertices_obb);

	glBegin(GL_LINES);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	for (uint i = 0; i < global_AABB.NumVerticesInEdgeList(); i++)
	{
		glVertex3f(vertices_aabb[i].x, vertices_aabb[i].y, vertices_aabb[i].z);
	}

	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	for (uint i = 0; i < global_OBB.NumVerticesInEdgeList(); i++)
	{
		glVertex3f(vertices_obb[i].x, vertices_obb[i].y, vertices_obb[i].z);
	}

	RELEASE(vertices_aabb);
	RELEASE(vertices_obb);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnd();
	glEnable(GL_LIGHTING);
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

float3 GameObject::GetEulerRotation() const
{
	return rotation_euler;
}

float4x4 GameObject::GetGlobalTransform() const
{
	return global_transform;
}

float3 GameObject::GetGlobalPosition() const
{
	float4x4 global_transform = GetGlobalTransform();
	return float3(global_transform[0][3], global_transform[1][3], global_transform[2][3]);
}

void GameObject::SetPosition(float3 new_position)
{
	position = new_position;
	UpdateTransformMatrix();
}

void GameObject::SetScale(float3 new_scale)
{
	scale = new_scale;
	UpdateTransformMatrix();

	//Getting normals sign
	float result = scale.x * scale.y * scale.z;
	flipped_normals = result >= 0 ? false : true;
}

void GameObject::SetRotation(float3 euler_angles)
{
	float3 delta = (euler_angles - rotation_euler) * DEGTORAD;
	Quat quaternion_rotation = Quat::FromEulerXYZ(delta.x, delta.y, delta.z);
	rotation = rotation * quaternion_rotation;
	rotation_euler = euler_angles;
	//rotation_euler *= RADTODEG;
	UpdateTransformMatrix();
}

void GameObject::ResetTransform()
{
	position = float3::zero;
	scale = float3::one;
	rotation = Quat::identity;

	UpdateEulerAngles();
	UpdateTransformMatrix();

	//Getting normals sign
	float result = scale.x * scale.y * scale.z;
	flipped_normals = result >= 0 ? false : true;
}

void GameObject::UpdateTransformMatrix()
{
	transform = float4x4::FromTRS(position, rotation, scale);
	UpdateGlobalTransform();
}

void GameObject::UpdateGlobalTransform()
{
	global_transform = parent ? parent->global_transform * transform : transform;
	global_transformT = global_transform.Transposed();

	UpdateAABB();

	for (uint i = 0; i < childs.size(); i++)
	{
		childs[i]->UpdateGlobalTransform();
	}
}

void GameObject::UpdateEulerAngles()
{
	rotation_euler = rotation.ToEulerXYZ();
	rotation_euler *= RADTODEG;
}

void GameObject::UpdateAABB()
{
	std::vector<Component*> meshes;
	GetComponents(Component::Type::Mesh, meshes);
	if (meshes.size())
	{
		const AABB aabb = ((C_Mesh*)meshes[0])->GetAABB();
		global_OBB = aabb;
		global_OBB.Transform(global_transform);

		global_AABB.SetNegativeInfinity();
		global_AABB.Enclose(global_OBB);
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
	Component* new_component = NULL;
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
			std::vector<Component*> mesh;
			GetComponents(Component::Type::Mesh, mesh);
			if (!mesh.empty())
			{
				new_component = ((C_Mesh*)mesh[0])->CreateMaterial();
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
	case(Component::Type::Mesh):
		{
			if (!HasComponent(Component::Type::Mesh))
			{
				components.push_back(component);
				component->gameObject = this;
				UpdateAABB();
			}
			break;
		}
	case(Component::Type::Material):
		{
			std::vector<Component*> mesh;
			GetComponents(Component::Type::Mesh, mesh);
			if (mesh.size() > 0)
			{
				((C_Mesh*)mesh[0])->AddMaterial((C_Material*)component);
			}
			break;
		}
	default:
		{
			components.push_back(component);
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

void GameObject::GetComponents(Component::Type type, std::vector<Component*>& vec)
{
	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type)
		{
			vec.push_back(components[i]);
		}
	}
}