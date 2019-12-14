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
	//Create a vertex array object which will hold all buffer objects
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Create a vertex buffer object to hold vertex positions
	glGenBuffers(1, &buffers[b_vertices]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[b_vertices]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffersSize[b_vertices] * 3, vertices, GL_STATIC_DRAW);

	//Create an element buffer object to hold indices
	glGenBuffers(1, &buffers[b_indices]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[b_indices]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * buffersSize[b_indices], indices, GL_STATIC_DRAW);

	//Set the vertex attrib pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Create the array buffer for tex coords and enable attrib pointer
	if (buffersSize[b_tex_coords] > 0)
	{
		glGenBuffers(1, &buffers[b_normals]);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[b_normals]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffersSize[b_tex_coords] * 2, tex_coords, GL_STATIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
	}

	//Create the array buffer for normals and enable attrib pointer
	if (buffersSize[b_normals] > 0)
	{
		glGenBuffers(1, &buffers[b_normals]);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[b_normals]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffersSize[b_normals] * 3, normals, GL_STATIC_DRAW);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
	}
	
	glBindVertexArray(0);
}

void R_Mesh::FreeMemory()
{
	//glDeleteBuffers(max_buffer_type, buffers);
}