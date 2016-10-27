#include "C_Mesh.h"
#include "OpenGL.h"
#include "GameObject.h"

C_Mesh::C_Mesh() : Component(Type::Mesh, nullptr)
{
}

C_Mesh::C_Mesh(GameObject* new_GameObject) : Component(Type::Mesh, new_GameObject)
{
}

C_Mesh::~C_Mesh()
{
	for (uint i = 0; i < materials.size(); i++)
	{
		if (materials[i] != nullptr)
		{
			RELEASE(materials[i]);
		}
	}
	materials.clear();
}

void C_Mesh::ReleaseBuffers()
{
	if (id_vertices != 0)
		glDeleteBuffers(1, (GLuint*)&id_vertices);

	if (id_indices != 0)
		glDeleteBuffers(1, (GLuint*)&id_indices);

	if (id_normals != 0)
		glDeleteBuffers(1, (GLuint*)&id_normals);

	if (id_tex_coords != 0)
		glDeleteBuffers(1, (GLuint*)&id_tex_coords);
}

void C_Mesh::LoadData(char* path)
{

}

void C_Mesh::LoadBuffers()
{
	glGenBuffers(1, (GLuint*)&id_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*)&id_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, indices, GL_STATIC_DRAW);

	if (num_normals > 0)
	{
		glGenBuffers(1, (GLuint*)&id_normals);
		glBindBuffer(GL_ARRAY_BUFFER, id_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_normals * 3, normals, GL_STATIC_DRAW);
	}

	if (num_tex_coords > 0)
	{
		glGenBuffers(1, (GLuint*)&id_tex_coords);
		glBindBuffer(GL_ARRAY_BUFFER, id_tex_coords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_tex_coords * 2, tex_coords, GL_STATIC_DRAW);
	}
}

void C_Mesh::UpdateAABB()
{
	local_bounds.SetNegativeInfinity();
	local_bounds.Enclose((math::vec*)vertices, num_vertices);
}

void C_Mesh::AddMaterial(C_Material* material)
{
	//TMP: we could have multiple materials in one mesh
	if (materials.empty())
		materials.push_back(material);
}

Component* C_Mesh::CreateMaterial()
{
	Component* component = nullptr;
	if (materials.empty())
	{
		component = new C_Material(gameObject);
		materials.push_back((C_Material*)component);
	}
	return component;
}

void C_Mesh::RemoveMaterial(C_Material* material)
{
	std::vector<C_Material*>::iterator position = std::find(materials.begin(), materials.end(), material);
	materials.erase(position);
}

const C_Material* C_Mesh::GetMaterial(uint position) const
{
	if (materials.empty() || materials.size() <= position)
		return nullptr;

	return materials[position];
}

uint C_Mesh::GetMaterialsSize() const
{
	return materials.size();
}

const AABB& C_Mesh::GetAABB() const
{
	return local_bounds;
}

const AABB& C_Mesh::GetGlobalAABB() const
{
	return global_bounds;
}
const OBB& C_Mesh::GetGlobalOBB() const
{
	return obb;
}

Component::Type C_Mesh::GetType()
{
	return Component::Type::Mesh;
}

void C_Mesh::OnUpdateTransform(const float4x4& global, const float4x4& parent_global)
{
	obb = local_bounds;
	obb.Transform(global);

	global_bounds.SetNegativeInfinity();
	global_bounds.Enclose(obb);
}