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
	C_Mesh();
	C_Mesh(GameObject* new_GameObject);
	~C_Mesh();
	void ReleaseBuffers();

	void LoadData(char* path);
	void LoadBuffers();

	void Draw(bool shaded, bool wireframe);
	void DrawAABB();

	void UpdateAABB();

	void AddMaterial(C_Material*);
	Component* CreateMaterial();
	void RemoveMaterial(C_Material*);

	const C_Material* GetMaterial(uint position) const;
	uint GetMaterialsSize() const;

	const AABB& GetAABB() const;

	static Component::Type GetType();

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

private:
	std::vector<C_Material*> materials;
	AABB bounds;
};

#endif

