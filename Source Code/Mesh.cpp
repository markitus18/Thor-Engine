#include "Mesh.h"
#include "OpenGL.h"
#include "GameObject.h"

Mesh::Mesh(GameObject* new_GameObject)
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

	if (num_normals > 0)
	{
		glGenBuffers(1, (GLuint*)&id_normals);
		glBindBuffer(GL_ARRAY_BUFFER, id_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_normals * 3, normals, GL_STATIC_DRAW);

		glGenBuffers(1, (GLuint*)&id_flipped_normals);
		glBindBuffer(GL_ARRAY_BUFFER, id_flipped_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_normals * 3, flipped_normals, GL_STATIC_DRAW);
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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	if (gameObject->IsSelected() || gameObject->IsParentSelected())
	{
		if (!gameObject->IsSelected())
		{
			glColor3f(0.51, 0.58, 0.68);
			glLineWidth(1);
		}

		else
		{
			glColor3f(0.81, 0.88, 0.98);
			glLineWidth(2);
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glLineWidth(1);
		glColor4f(1, 1, 1, 1);
	}

	if (num_normals > 0)
	{
		if (gameObject->HasFlippedNormals())
		{
			glFrontFace(GL_CW);
		}
		glBindBuffer(GL_ARRAY_BUFFER, id_normals);
		glNormalPointer(GL_FLOAT, 0, NULL);
	}

	if (num_tex_coords > 0)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, id_tex_coords);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}

	glBindTexture(GL_TEXTURE_2D, 2);
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);


	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glFrontFace(GL_CCW);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}