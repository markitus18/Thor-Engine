#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "MathGeoLib/src/MathBuildConfig.h"
#include "MathGeoLib/src/MathGeoLib.h"

#include <vector>

#include "C_Mesh.h"
#include "C_Material.h"
#include "C_Camera.h"
#include "C_Transform.h"
#include "Component.h"


class GameObject
{
public:
	GameObject();
	GameObject(GameObject* parent,  const char* name = "No name", const float3& translation = float3::zero, const float3& scale = float3::one, const Quat& rotation = Quat::identity);
	~GameObject();

	void Draw(bool shaded, bool wireframe);
	void DrawAABB();
	//TODO: move into camera compoment or debug comp
	void DrawCamera(C_Camera* camera);

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
	void UpdateGlobalTransform();
	void UpdateEulerAngles();
	void UpdateAABB();
	//TODO: TMP
	void UpdateCamera();

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

	template<typename RetComponent>
	RetComponent* GetComponent()
	{
		Component::Type type = RetComponent::GetType();
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->GetType() == type)
			{
				return ((RetComponent*)(components[i]));
			}
		}
		return NULL;
	}



public:
	std::string					name;

	GameObject*					parent = nullptr;
	std::vector<GameObject*>	childs;
	bool						active = true;
	AABB						global_AABB;
	OBB							global_OBB;

private:
	float4x4	transform;
	float4x4	global_transform;
	float4x4	global_transformT;

	float3		position;
	float3		scale;
	Quat		rotation;
	float3		rotation_euler;
	bool		flipped_normals = false;

	C_Transform* transform_2;
	std::vector<Component*> components;

	bool selected = false;
};

#endif
