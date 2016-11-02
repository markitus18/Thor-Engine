#ifndef __C_TRANSFORM_H__
#define __C_TRANSFORM_H__

#include "Component.h"

#include "Globals.h"
#include "MathGeoLib\src\MathGeoLib.h"

#include <vector>
#include <list>

class GameObject;

class C_Transform : public Component
{
public:
	C_Transform(GameObject* new_GameObject, float3 position = float3::zero, Quat rotation = Quat::identity, float3 scale = float3::one);
	~C_Transform();

	float4x4	GetTransform() const;
	float3		GetPosition() const;
	Quat		GetQuatRotation() const;
	float3		GetEulerRotation() const;
	float3		GetScale() const;
	float4x4	GetGlobalTransform() const;
	float4x4	GetGlobalTransformT() const;
	float3		GetGlobalPosition() const;

	void SetPosition(float3 position);
	void SetScale(float3 scale);
	void SetEulerRotation(float3 euler_angles);

	void Reset();

	void OnUpdateTransform(const float4x4& global, const float4x4& parent_global = float4x4::identity);

	void Save();
	void Load();
	static Component::Type GetType();

private:
	void UpdateLocalTransform();
	void UpdateEulerAngles();

private:
	float4x4	transform;
	float4x4	global_transform;
	float4x4	global_transformT;

	float3		position;
	float3		scale;
	Quat		rotation;
	float3		rotation_euler;

public:
	bool		flipped_normals = false;
	bool		transform_updated = true;
};

#endif //C_TRANSFORM_H__

