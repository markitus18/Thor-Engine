#ifndef __C_TRANSFORM_H__
#define __C_TRANSFORM_H__

#include "Component.h"

#include "Globals.h"
#include "MathGeoLib\src\MathGeoLib.h" //TODO: include only the used structures

#include <vector>
#include <list>

class GameObject;

class C_Transform : public Component
{
public:
	C_Transform(GameObject* new_GameObject, float3 position = float3::zero, Quat rotation = Quat::identity, float3 scale = float3::one);
	C_Transform(GameObject* new_GameObject, const float4x4& transform = float4x4::identity);

	~C_Transform();

	inline float4x4		GetTransform() const { return transform; }
	inline float3		GetPosition() const { return position; }
	inline Quat			GetQuatRotation() const { return rotation; }
	inline float3		GetEulerRotation() const { return rotation_euler; }
	inline float3		GetScale() const { return scale; }
	inline float4x4		GetGlobalTransform() const { return global_transform; }
	inline float4x4		GetGlobalTransformT() const { return global_transformT; }
	float3				GetGlobalPosition() const;

	void SetPosition(float3 position);
	void SetScale(float3 scale);
	void SetQuatRotation(Quat rotation);
	void SetEulerRotation(float3 euler_angles);
	void SetGlobalTransform(float4x4 transform);

	void LookAt(float3 target);

	void Reset();

	void OnUpdateTransform(const float4x4& global, const float4x4& parent_global = float4x4::identity);

	void Serialize(Config& config) override;

	static inline Type GetType() { return Type::Transform; };

private:
	void UpdateLocalTransform();
	void UpdateTRS();
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

