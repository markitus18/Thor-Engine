#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "C_Mesh.h"
#include "C_Material.h"

#include "MathGeoLib/src/MathBuildConfig.h"
#include "MathGeoLib/src/MathGeoLib.h"

#include <vector>

class GameObject
{
public:
	GameObject();
	//GameObject(const GameObject* parent, const char* name);
	GameObject(GameObject* parent,  const char* name = "No name", const float3& translation = float3::zero, const float3& scale = float3::one, const Quat& rotation = Quat::identity);
	~GameObject();

	void Draw(bool shaded, bool wireframe);

	float3		GetPosition() const;
	float3		GetScale() const;
	Quat		GetQuatRotation() const;
	float3		GetEulerRotation() const;
	float4x4	GetGlobalTransform() const;
	float3		GetGlobalPosition() const;

	void SetPosition(float3 new_position);
	void SetScale(float3 new_scale);
	void SetRotation(float3 euler_angles);

	void ResetTransform();

	void UpdateTransformMatrix();
	void UpdateEulerAngles();

	bool HasFlippedNormals() const;

	//Selection methods
	void Select();
	void Unselect();
	bool IsSelected() const;
	bool IsParentSelected() const;

	//Component management
	Component* CreateComponent(Component::Type type);
	void AddComponent(Component* component);
	bool HasComponent(Component::Type type);
	void GetComponents(Component::Type type, std::vector<Component*>& vec);

public:
	std::string					name;

	GameObject*					parent = NULL;
	std::vector<GameObject*>	childs;
	bool						active = true;

private:
	float4x4	transform;

	float3		position;
	float3		scale;
	Quat		rotation;
	float3		rotation_euler;
	bool		flipped_normals = false;

	std::vector<Component*> components;

	bool selected = false;
};

#endif
