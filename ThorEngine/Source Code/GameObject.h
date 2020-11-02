#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "MathGeoLib/src/MathBuildConfig.h"
#include "MathGeoLib/src/MathGeoLib.h"

//#include "MathGeoLib/src/Geometry/AABB.h"


#include "TreeNode.h"

#include <vector>
#include "Component.h"

class C_Transform;
class Config;
class Scene;

typedef unsigned __int64 RenderingFlags;

class GameObject : public TreeNode
{
public:
	GameObject();
	GameObject(GameObject* parent,  const char* name = "No name", const float3& translation = float3::zero, const Quat& rotation = Quat::identity, const float3& scale = float3::one);
	GameObject(GameObject* parent, const float4x4& transform, const char* name = "No name");

	~GameObject();

	void Destroy();
	void Update(float dt);
	void Draw(RenderingFlags flags);
	void DrawResursive(RenderingFlags flags);

	void OnUpdateTransform();
	const AABB& GetAABB() const;
	const OBB& GetOBB() const;
	bool HasFlippedNormals() const;

	void Serialize(Config& config);

	void SetParent(GameObject* gameObject, GameObject* next = nullptr, bool worldPositionStays = true);
	bool IsParentActive() const;
	void RemoveChild(GameObject* gameObject);
	bool HasChildInTree(GameObject* gameObject) const;

	void CollectChilds(std::vector<GameObject*>& vector);
	void CollectChilds(std::vector<const GameObject*>& vector) const;
	GameObject* FindChildByName(const char* name) const;
	GameObject* GetChild(uint index) const;

	int GetChildIndex(GameObject* gameObject) const;
	std::vector<TreeNode*> GetChilds() const;
	TreeNode* GetParentNode() const;
	bool IsNodeActive() const;
	bool DrawTreeNode() const;
	void SetParentNode(TreeNode* parent, TreeNode* next);

	const char* GetName() const;
	unsigned long long GetID() const;

	void SetStatic(bool isStatic, bool setChildren = false);

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

	Scene*						sceneOwner = nullptr;

	bool						active = true;
	bool						isStatic = false;

	unsigned long long			uid = 0;
	 
private:
	bool						flipped_normals = false;

	C_Transform*				transform = nullptr;
	std::vector<Component*>		components; //TODO: move into map? 

	AABB						aabb;
	OBB							obb;
};

#endif
