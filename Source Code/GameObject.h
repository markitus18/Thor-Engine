#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Mesh.h"
#include "MathGeoLib/src/MathBuildConfig.h"
#include "MathGeoLib/src/MathGeoLib.h"

#include <vector>

class GameObject
{
public:
	GameObject();
	GameObject(const GameObject* parent, const char* name);
	GameObject(const GameObject* parent, const float3& translation, const float3& scale, const Quat& rotation, const char* name);
	~GameObject();

	void Draw();

	const float3 GetPosition();
	const float3 GetScale();
	const Quat GetQuatRotation();

	void Select();
	void Unselect();
	bool IsSelected();
public:
	std::string					name;
	const GameObject*			parent = NULL;
	std::vector<GameObject*>	childs;

	Mesh* mesh = NULL;

private:
	float3		position;
	float3		scale;
	Quat		rotation;
	float4x4	transform;

	bool selected = false;



};




#endif
