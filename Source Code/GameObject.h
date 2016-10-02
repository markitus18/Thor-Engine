#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Mesh.h"
#include "MathGeoLib/src/MathBuildConfig.h"
#include "MathGeoLib/src/MathGeoLib.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

public:
	float4x4 transform;
	Mesh mesh;
};




#endif
