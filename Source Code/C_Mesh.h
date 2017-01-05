#ifndef __MESH_H__
#define __MESH_H__

#include "Globals.h"
#include "Component.h"
#include "MathGeoLib\src\MathGeoLib.h"

#include <vector>
#include <list>

class GameObject;
class C_Bone;
class R_Mesh;

class C_Mesh : public Component
{
public:
	C_Mesh();
	C_Mesh(GameObject* new_GameObject);
	~C_Mesh();

	void AddBone(C_Bone* bone);
	const AABB& GetAABB() const;
	static Component::Type GetType();

	void StartBoneDeformation();
	void DeformAnimMesh();

public:
	std::vector<C_Bone*> bones;
	R_Mesh* animMesh = nullptr;

};

#endif

