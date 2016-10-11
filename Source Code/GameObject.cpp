#include "GameObject.h"
#include "OpenGL.h"
#include "Globals.h"

GameObject::GameObject()
{
}
//GameObject::GameObject(const GameObject* new_parent, const char* new_name) : position(float3::zero), scale(float3::one), rotation(Quat::identity), name(new_name)
//{
//	parent = new_parent;
//	UpdateEulerAngles();
//	UpdateTransformMatrix();
//}

GameObject::GameObject(const GameObject* new_parent, const char* new_name, const float3& translation, const float3& scale, const Quat& rotation) : position(translation), scale(scale),
						rotation(rotation), name(new_name)
{
	parent = new_parent;
	UpdateEulerAngles();
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
		if (childs[i]->active)
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

float3 GameObject::GetEulerRotation() const
{
	return rotation_euler;
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
	transform.Transpose();
}

void GameObject::UpdateEulerAngles()
{
	rotation_euler = rotation.ToEulerXYZ();
	rotation_euler *= RADTODEG;
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

void GameObject::AddMesh(Mesh* new_mesh)
{
	mesh = new_mesh;
	mesh->gameObject = this;
}