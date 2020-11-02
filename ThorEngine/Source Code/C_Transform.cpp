#include "C_Transform.h"
#include "Component.h"
#include "GameObject.h"

C_Transform::C_Transform(GameObject* new_GameObject, float3 position, Quat rotation, float3 scale) : Component(Component::Type::Transform, gameObject, false), position(position), rotation(rotation), scale(scale)
{
	transform = float4x4::FromTRS(position, rotation, scale);
	UpdateEulerAngles();
}

C_Transform::C_Transform(GameObject* new_GameObject, const float4x4& transform) : Component(Component::Type::Transform, gameObject, false)
{
	this->transform = transform;
	transform.Decompose(position, rotation, scale);
	UpdateEulerAngles();
}

C_Transform::~C_Transform()
{

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
	float result = scale.x * scale.y * scale.z;
	flipped_normals = result >= 0 ? false : true;
}

void C_Transform::SetQuatRotation(Quat rotation)
{
	this->rotation = rotation;
	UpdateEulerAngles();
	UpdateLocalTransform();
}

void C_Transform::SetEulerRotation(float3 euler_angles)
{
	float3 delta = (euler_angles - rotation_euler) * DEGTORAD;
	Quat quaternion_rotation = Quat::FromEulerXYZ(delta.x, delta.y, delta.z);
	rotation = rotation * quaternion_rotation;
	rotation_euler = euler_angles;
	UpdateLocalTransform();
}

void C_Transform::SetGlobalTransform(float4x4 transform)
{
	float4x4 localTransform = gameObject->parent->GetComponent<C_Transform>()->GetGlobalTransform().Inverted() * transform;
	this->transform = localTransform;
	global_transform = transform;
	global_transformT = global_transform.Transposed();
	transform_updated = true;
}

void C_Transform::LookAt(float3 target)
{
	float3 dir = target - position;

	float3 fwd = dir.Normalized();
	float3 right = float3::unitY.Cross(fwd).Normalized();
	float3 up = fwd.Cross(right).Normalized();

	transform.SetCol3(0, right.x, right.y, right.z);
	transform.SetCol3(1, up.x, up.y, up.z);
	transform.SetCol3(2, fwd.x, fwd.y, fwd.z);

	transform_updated = true;
}

void C_Transform::Reset()
{
	position = float3::zero;
	scale = float3::one;
	rotation = Quat::identity;

	UpdateEulerAngles();
	UpdateLocalTransform();

	flipped_normals = false;
}

void C_Transform::OnUpdateTransform(const float4x4& global, const float4x4& parent_global)
{
	global_transform = parent_global * transform;
	global_transformT = global_transform.Transposed();
	UpdateTRS();
	transform_updated = false;
}

void C_Transform::Serialize(Config& config)
{
	Component::Serialize(config);
	if (config.isSaving)
	{
		config.SetArray("Translation").AddFloat3(GetPosition());
		config.SetArray("Rotation").AddQuat(GetQuatRotation());
		config.SetArray("Scale").AddFloat3(GetScale());
	}
	else
	{

		position = config.GetArray("Translation").GetFloat3(0, float3::zero);
		rotation = config.GetArray("Rotation").GetQuat(0, Quat::identity);
		scale = config.GetArray("Scale").GetFloat3(0, float3::one);

		transform = float4x4::FromTRS(position, rotation, scale);
		transform_updated = true;
		UpdateEulerAngles();
	}
}

void C_Transform::UpdateLocalTransform()
{
	transform = float4x4::FromTRS(position, rotation, scale);
	transform_updated = true;
}

void C_Transform::UpdateTRS()
{
	transform.Decompose(position, rotation, scale);
	UpdateEulerAngles();
}

void C_Transform::UpdateEulerAngles()
{
	rotation_euler = rotation.ToEulerXYZ();
	rotation_euler *= RADTODEG;
}
