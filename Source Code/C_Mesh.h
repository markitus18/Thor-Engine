#ifndef __MESH_H__
#define __MESH_H__

#include "Globals.h"
#include "Component.h"
#include "MathGeoLib\src\MathGeoLib.h"

#include <vector>
#include <list>

class GameObject;
class C_Material;

class C_Mesh : public Component
{
public:
	//TODO: organize functions order
	C_Mesh();
	C_Mesh(GameObject* new_GameObject);
	~C_Mesh();
	void ReleaseBuffers();

	void LoadData(char* path);
	void LoadBuffers();

	void UpdateAABB();

	void AddMaterial(C_Material*);
	Component* CreateMaterial();
	void RemoveMaterial(C_Material*);

	const C_Material* GetMaterial(uint position) const;
	uint GetMaterialsSize() const;

	const AABB& GetAABB() const;
	const AABB& GetGlobalAABB() const;
	const OBB& GetGlobalOBB() const;

	static Component::Type GetType();

	void OnUpdateTransform(const float4x4& global, const float4x4& parent_global = float4x4::identity);

public:
	//Vertices data
	uint	id_vertices = 0;
	uint	num_vertices = 0;
	uint*	indices = nullptr;

	//Indices data
	uint	id_indices = 0;
	uint	num_indices = 0;
	float*	vertices = nullptr;

	//Normals data
	uint	id_normals = 0;
	uint	num_normals = 0;
	float*	normals = nullptr;

	//Texture coords
	uint	id_tex_coords = 0;
	uint	num_tex_coords = 0;
	float*	tex_coords = nullptr;

	bool flipped_normals = false;
	std::vector<C_Material*> materials;

private:

	AABB	local_bounds;
	OBB		obb;
	AABB	global_bounds;
};

#endif

