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
	//TODO: think a way of doing it on renderer. Store a vector with all loaded meshes?
	ReleaseBuffers();
}

void C_Mesh::ReleaseBuffers()
{
	if (id_vertices != 0)
	{
		glDeleteBuffers(1, (GLuint*)&id_vertices);
		LOG("ID Vertices: %i", id_vertices);
	}

	if (id_indices != 0)
	{
		glDeleteBuffers(1, (GLuint*)&id_indices);
		LOG("ID Indices: %i", id_indices);
	}

	if (id_normals != 0)
	{
		glDeleteBuffers(1, (GLuint*)&id_normals);
		LOG("ID Normals: %i", id_normals);
	}

	if (id_tex_coords != 0)
	{
		glDeleteBuffers(1, (GLuint*)&id_tex_coords);
		LOG("ID TexCoords: %i", id_tex_coords);
	}
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

void C_Mesh::CreateAABB()
{
	aabb.SetNegativeInfinity();
	aabb.Enclose((math::vec*)vertices, num_vertices);
}

const AABB& C_Mesh::GetAABB() const
{
	return aabb;
}

Component::Type C_Mesh::GetType()
{
	return Component::Type::Mesh;
}

void C_Mesh::Save()
{

}

void C_Mesh::Load()
{

}