#include "C_Transform.h"
#include "Component.h"


C_Transform::C_Transform(GameObject* new_GameObject, float3 position, Quat rotation, float3 scale) : Component(Component::Type::Transform, gameObject), position(position), rotation(rotation), scale(scale)
{
	transform = float4x4::FromTRS(position, rotation, scale);
}

C_Transform::~C_Transform()
{

}

float4x4 C_Transform::GetTransform() const
{
	return transform;
}

float3 C_Transform::GetPosition() const
{
	return position;
}

Quat C_Transform::GetQuatRotation() const
{
	return rotation;
}

float3 C_Transform::GetEulerRotation() const
{
	return rotation_euler;
}

float3 C_Transform::GetScale() const
{
	return scale;
}

float4x4 C_Transform::GetGlobalTransform() const
{
	return global_transform;
}

float3 C_Transform::GetGlobalPosition() const
{
	float4x4 global_transform = GetGlobalTransform();
	return float3(global_transform[0][3], global_transform[1][3], global_transform[2][3]);
}

void C_Transform::SetPosition(float3 new_position)
{
	position = new_position;
	UpdateLocalTransform();
}

void C_Transform::SetScale(float3 new_scale)
{
	scale = new_scale;
	UpdateLocalTransform();

	//Getting normals sign
	//float result = scale.x * scale.y * scale.z;
	//flipped_normals = result >= 0 ? false : true;
}

void C_Transform::SetEulerRotation(float3 euler_angles)
{
	float3 delta = (euler_angles - rotation_euler) * DEGTORAD;
	Quat quaternion_rotation = Quat::FromEulerXYZ(delta.x, delta.y, delta.z);
	rotation = rotation * quaternion_rotation;
	rotation_euler = euler_angles;
	//rotation_euler *= RADTODEG;
	UpdateLocalTransform();
}

void C_Transform::UpdateLocalTransform()
{
	transform = float4x4::FromTRS(position, rotation, scale);

}