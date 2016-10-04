#include "Mesh.h"
#include "OpenGL.h"

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
}

void Mesh::LoadData(char* path)
{

}

void Mesh::LoadBuffers()
{
	glGenBuffers(1, (GLuint*)&id_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STATIC_DRAW);


	glGenBuffers(1, (GLuint*)&id_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, indices, GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*)&id_normals);
	glBindBuffer(GL_ARRAY_BUFFER, id_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * num_normals * 3, normals, GL_STATIC_DRAW);

	if (num_normals > 0)
	{
		glGenBuffers(1, (GLuint*)&id_normals);
		glBindBuffer(GL_ARRAY_BUFFER, id_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * num_normals * 3, normals, GL_STATIC_DRAW);
	}

	if (num_tex_coords > 0)
	{
		glGenBuffers(1, (GLuint*)&id_tex_coords);
		glBindBuffer(GL_ARRAY_BUFFER, id_tex_coords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_tex_coords, tex_coords, GL_STATIC_DRAW);
	}
}

void Mesh::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	if (num_normals > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, id_normals);
		glNormalPointer(GL_FLOAT, 0, NULL);
	}

	if (num_tex_coords > 0)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, id_tex_coords);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}

	glBindBuffer(GL_ARRAY_BUFFER, id_normals);
	glNormalPointer(GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);

	glBindTexture(GL_TEXTURE_2D, 2);
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}