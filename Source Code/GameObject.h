#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "MathGeoLib/src/MathBuildConfig.h"
#include "MathGeoLib/src/MathGeoLib.h"

#include <vector>
#include "Component.h"

#include "C_Mesh.h"
#include "C_Material.h"
#include "C_Camera.h"
#include "C_Transform.h"
#include "C_Animation.h"
#include "C_Bone.h"

class C_Transform;

class GameObject
{
public:
	GameObject();
	GameObject(GameObject* parent,  const char* name = "No name", const float3& translation = float3::zero, const float3& scale = float3::one, const Quat& rotation = Quat::identity);
	~GameObject();

	void Update(float dt);
	void Draw(bool shaded, bool wireframe, bool drawBox, bool drawBoxSelected);
	void DrawResursive(bool shaded, bool wireframe, bool drawBox, bool drawBosSelected);

	void OnUpdateTransform();
	const AABB& GetAABB() const;
	const OBB& GetOBB() const;
	bool HasFlippedNormals() const;

	void SetParent(GameObject* gameObject, bool worldPositionStays = true);
	bool IsParentActive() const;
	void RemoveChild(GameObject* gameObject);

	void CollectChilds(std::vector<GameObject*>& vector);
	GameObject* FindChildByName(const char* name) const;
	GameObject* GetChild(uint index) const;
	int GetChildIndex(GameObject* gameObject) const;

	//Selection methods -----------------------------------
	void Select();
	void Unselect();
	bool IsSelected() const;
	bool IsParentSelected() const;
	//EndOf Selection methods -----------------------------

	void SetStatic(bool isStatic);

	//Component management --------------------------------
	Component* CreateComponent(Component::Type type);
	void AddComponent(Component* component);
	bool HasComponent(Component::Type type);

	template<typename RetComponent>
	const RetComponent* GetComponent() const
	{
		Component::Type type = RetComponent::GetType();
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->GetType() == type)
			{
				return ((RetComponent*)(components[i]));
			}
		}
		return nullptr;
	}

	template<typename RetComponent>
	RetComponent* GetComponent()
	{
		Component::Type type = RetComponent::GetType();
		if (type == Component::Type::Transform)
			return (RetComponent*)transform;
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->GetType() == type)
			{
				return ((RetComponent*)(components[i]));
			}
		}
		return nullptr;
	}

	template<typename RetComponent>
	bool GetComponents(std::vector<RetComponent*>& vector)
	{
		Component::Type type = RetComponent::GetType();
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->GetType() == type)
			{
				vector.push_back((RetComponent*)components[i]);
			}
		}
		return vector.empty() ? false : true;
	}

	const std::vector<Component*>& GetAllComponents() const
	{
		return components;
	}

	//EndOf Component management -------------------------

private:
	void UpdateAABB();
public:
	std::string					name;

	GameObject*					parent = nullptr;
	std::vector<GameObject*>	childs;

	//TODO: this for when prefabs (apply / revert) get implemented
	Resource*					link = nullptr;

	bool						active = true;
	bool						isStatic = false;

	unsigned long long			uid = 0;

	bool						beenSelected = false;
	bool						hierarchyOpen = false;

private:
	bool						flipped_normals = false;

	C_Transform*				transform = nullptr;
	std::vector<Component*>		components;

	AABB						aabb;
	OBB							obb;

	bool						selected = false;
};

#endif
