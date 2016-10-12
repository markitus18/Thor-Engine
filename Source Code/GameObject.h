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
	GameObject(const GameObject* parent,  const char* name = "No name", const float3& translation = float3::zero, const float3& scale = float3::one, const Quat& rotation = Quat::identity);
	~GameObject();

	void Draw();

	float3	GetPosition() const;
	float3	GetScale() const;
	Quat	GetQuatRotation() const;
	float3	GetEulerRotation() const;

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

	void AddMesh(C_Mesh* mesh);
	void AddMaterial(C_Material* material);

public:
	std::string					name;

	const GameObject*			parent = NULL;
	std::vector<GameObject*>	childs;
	bool						active = true;

	C_Mesh*		mesh = NULL;

private:
	float4x4	transform;

	float3		position;
	float3		scale;
	Quat		rotation;
	float3		rotation_euler;
	bool		flipped_normals = false;

	bool selected = false;
};

#endif
