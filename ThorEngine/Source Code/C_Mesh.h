#ifndef __MESH_H__
#define __MESH_H__

#include "Globals.h"
#include "Component.h"
#include "MathGeoLib\src\MathGeoLib.h"

#include <map>
#include <string>

class GameObject;
class R_Mesh;

class C_Mesh : public Component
{
public:
	C_Mesh();
	C_Mesh(GameObject* new_GameObject);
	~C_Mesh();

	virtual void Draw(RenderingSettings::RenderingFlags flags) override;

	const AABB& GetAABB() const;
	static inline Type GetType() { return Type::Mesh; };

	void StartBoneDeformation();
	void DeformAnimMesh();

	void GetBoneMapping(std::map<std::string, GameObject*>& boneMapping);

	void SetResource(Resource* resource);
	void SetResource(unsigned long long id);

	uint64 GetResourceID() const;

public:
	R_Mesh* animMesh = nullptr;
	GameObject* rootBone = nullptr;

	ResourceHandle<R_Mesh> rMeshHandle;
};

#endif

