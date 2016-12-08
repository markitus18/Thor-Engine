#include "R_Mesh.h"
#include "OpenGL.h"

R_Mesh::R_Mesh() : Resource(MESH)
{
	for (uint i = 0; i < max_buffer_type; i++)
	{
		buffers[i] = 0;
		buffersSize[i] = 0;
	}
}

R_Mesh::~R_Mesh()
{

}

void R_Mesh::CreateAABB()
{
	aabb.SetNegativeInfinity();
	aabb.Enclose((math::vec*)vertices, buffersSize[b_vertices]);
}

void R_Mesh::LoadOnMemory()
{
	glGenBuffers(1, (GLuint*)&buffers[b_vertices]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[b_vertices]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffersSize[b_vertices] * 3, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*)&buffers[b_indices]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[b_indices]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * buffersSize[b_indices], indices, GL_STATIC_DRAW);

	if (buffersSize[b_normals] > 0)
	{
		glGenBuffers(1, (GLuint*)&buffers[b_normals]);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[b_normals]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffersSize[b_normals] * 3, normals, GL_STATIC_DRAW);
	}

	if (buffersSize[b_tex_coords] > 0)
	{
		glGenBuffers(1, (GLuint*)&buffers[b_tex_coords]);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[b_tex_coords]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffersSize[b_tex_coords] * 2, tex_coords, GL_STATIC_DRAW);
	}
}

void R_Mesh::FreeMemory()
{
	//glDeleteBuffers(max_buffer_type, buffers);
}