#ifndef __MESH_H__
#define __MESH_H__

#include "Globals.h"
#include "Component.h"
#include "MathGeoLib\src\MathGeoLib.h"

#include <vector>
#include <list>

class GameObject;

class C_Mesh : public Component
{
public:
	//TODO: organize functions order
	C_Mesh();
	C_Mesh(GameObject* new_GameObject);
	~C_Mesh();

	const AABB& GetAABB() const;

	void Save();
	void Load();

	static Component::Type GetType();
};

#endif

