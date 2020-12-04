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

	// All transformation methods en up calling this function. Updates TRS from the new local transform and recursively updates the global transformations
	void SetLocalTransform(float4x4 newTransform);

	// Local setters
	inline void SetLocalPosition(float3 newPosition)	{ SetLocalTransform(newPosition, localRotation, localScale); }
	inline void SetLocalScale(float3 newScale)			{ SetLocalTransform(localPosition, localRotation, newScale); }
	inline void SetLocalRotation(Quat newRotation)		{ SetLocalTransform(localPosition, newRotation, localScale); }
	void SetLocalEulerRotation(float3 euler_angles);
	inline void SetLocalTransform(float3 position, Quat rotation, float3 scale) { SetLocalTransform(float4x4::FromTRS(position, rotation, scale)); }

	// Global setters
	void SetPosition(float3 newPosition);
	void SetRotationAxis(float3 x, float3 y, float3 z);
	void SetGlobalTransform(float4x4 transform);

	// Local accessors
	inline float4x4		GetLocalTransform() const { return localTransform; }
	inline float3		GetLocalPosition() const { return localPosition; }
	inline Quat			GetLocalRotation() const { return localRotation; }
	inline float3		GetLocalEulerRotation() const { return localRotationEuler; }
	inline float3		GetLocalScale() const { return localScale; }

	// Global accessors
	inline float4x4		GetTransform() const { return transform; }
	inline float4x4		GetTransformT() const { return transformT; }

	inline float3		GetPosition() const { return transform.TranslatePart(); }
	inline float3		GetFwd() const { return transform.WorldZ(); }
	inline float3		GetUp() const { return transform.WorldY(); }
	inline float3		GetRight() const { return transform.WorldX(); }

public:
	void LookAt(float3 target, float3 worldUp = float3::unitY);

	void Reset();

	void OnTransformUpdated() override;

	void Serialize(Config& config) override;

	static inline Type GetType() { return Type::Transform; };

private:
	void UpdateTransformHierarchy();

private:
	float4x4	localTransform;
	float4x4	transform;
	float4x4	transformT;

	float3		localPosition;
	float3		localScale;
	Quat		localRotation;
	float3		localRotationEuler;

public:
	bool		transform_updated = true;
};

#endif //C_TRANSFORM_H__

