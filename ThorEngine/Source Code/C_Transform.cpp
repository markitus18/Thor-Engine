#include "C_Transform.h"
#include "Component.h"
#include "GameObject.h"

C_Transform::C_Transform(GameObject* parent, float3 position, Quat rotation, float3 scale) : Component(Component::Type::Transform, parent, false),
						localPosition(position), localRotation(rotation), localScale(scale)
{
	SetLocalTransform(position, rotation, scale);
}

C_Transform::C_Transform(GameObject* parent, const float4x4& transform) : Component(Component::Type::Transform, parent, false)
{
	SetLocalTransform(transform);
}

C_Transform::~C_Transform()
{

}

void C_Transform::SetLocalEulerRotation(float3 euler_angles)
{
	float3 delta = (euler_angles - localRotationEuler) * DEGTORAD;
	Quat newRotation = localRotation * Quat::FromEulerXYZ(delta.x, delta.y, delta.z);
	SetLocalTransform(localPosition, newRotation, localScale);
}

void C_Transform::SetLocalTransform(float4x4 newTransform)
{
	localTransform = newTransform;
	localTransform.Decompose(localPosition, localRotation, localScale);
	localRotationEuler = localRotation.ToEulerXYZ() * RADTODEG;

	UpdateTransformHierarchy();
}

void C_Transform::SetGlobalTransform(float4x4 transform)
{
	// Recalculate local transform depending on the parent transform
	localTransform = transform;
	if (gameObject->parent)
		localTransform = gameObject->parent->GetComponent<C_Transform>()->GetTransform().Transposed() * transform;
	SetLocalTransform(localTransform);
}

void C_Transform::SetPosition(float3 newPosition)
{
	transform.SetTranslatePart(newPosition);
	SetGlobalTransform(transform);
}

void C_Transform::SetRotationAxis(float3 x, float3 y, float3 z)
{
	transform.SetCol3(0, x);	
	transform.SetCol3(1, y);
	transform.SetCol3(2, z);

	SetGlobalTransform(transform);
}

void C_Transform::LookAt(float3 target)
{
	float3 dir = target - GetPosition();

	float3 fwd = dir.Normalized();
	float3 right = float3::unitY.Cross(fwd).Normalized();
	float3 up = fwd.Cross(right).Normalized();

	SetRotationAxis(right, up, fwd);
}

void C_Transform::Reset()
{
	SetLocalTransform(float4x4::identity);
}

void C_Transform::OnTransformUpdated()
{
	transform_updated = false;
}

void C_Transform::Serialize(Config& config)
{
	Component::Serialize(config);
	if (config.isSaving)
	{
		config.SetArray("Translation").AddFloat3(localPosition);
		config.SetArray("Rotation").AddQuat(localRotation);
		config.SetArray("Scale").AddFloat3(localScale);
	}
	else
	{

		localPosition = config.GetArray("Translation").GetFloat3(0, float3::zero);
		localRotation = config.GetArray("Rotation").GetQuat(0, Quat::identity);
		localScale = config.GetArray("Scale").GetFloat3(0, float3::one);

		localTransform = float4x4::FromTRS(localPosition, localRotation, localScale);
		transform_updated = true;

		localRotationEuler = localRotation.ToEulerXYZ() * RADTODEG;
	}
}

void C_Transform::UpdateTransformHierarchy()
{
	transform = gameObject->parent ? gameObject->parent->GetComponent<C_Transform>()->GetTransform() * localTransform : localTransform;
	transformT = transform.Transposed();
	transform_updated = true;

	// Recursively update child transforms
	for (uint i = 0; i < gameObject->childs.size(); ++i)
	{
		gameObject->childs[i]->GetComponent<C_Transform>()->UpdateTransformHierarchy();
	}
}