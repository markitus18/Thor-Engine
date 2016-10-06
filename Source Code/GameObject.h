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
	GameObject(const GameObject* parent);
	GameObject(const GameObject* parent, const float3& translation, const float3& scale, const Quat& rotation);
	~GameObject();

	void Draw();
public:
	
	float3		position;
	float3		scale;
	Quat		rotation;
	float4x4	transform;

	const GameObject* parent = NULL;
	std::vector<GameObject*> childs;

	Mesh* mesh = NULL;

};




#endif
