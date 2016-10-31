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

	void Update();
	void Draw(bool shaded, bool wireframe);

	void OnUpdateTransform();

	bool HasFlippedNormals() const;

	//Selection methods -----------------------------------
	void Select();
	void Unselect();
	bool IsSelected() const;
	bool IsParentSelected() const;
	//EndOf Selection methods -----------------------------

	//Component management --------------------------------
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

	template<typename RetComponent>
	bool GetComponents(std::vector<RetComponent*> vector)
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
	//EndOf Component management -------------------------

public:
	std::string					name;

	GameObject*					parent = nullptr;
	std::vector<GameObject*>	childs;
	bool						active = true;

private:
	bool						flipped_normals = false;

	C_Transform*				transform = nullptr;
	std::vector<Component*>		components;

	bool						selected = false;
};

#endif
