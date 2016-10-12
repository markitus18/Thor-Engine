#include "C_Mesh.h"
#include "OpenGL.h"
#include "GameObject.h"

C_Mesh::C_Mesh(GameObject* new_GameObject) : Component(Type::Mesh, new_GameObject)
{

}

C_Mesh::~C_Mesh()
{

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

		glGenBuffers(1, (GLuint*)&id_flipped_normals);
		glBindBuffer(GL_ARRAY_BUFFER, id_flipped_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_normals * 3, flipped_normals, GL_STATIC_DRAW);
	}

	if (num_tex_coords > 0)
	{
		glGenBuffers(1, (GLuint*)&id_tex_coords);
		glBindBuffer(GL_ARRAY_BUFFER, id_tex_coords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_tex_coords * 2, tex_coords, GL_STATIC_DRAW);
	}
}

void C_Mesh::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	//Removed temporaly: game objects will use buffers, not mesh itself
	
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
		//Removed temporaly: game objects will use buffers, not mesh itself
		
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

	if (!materials.empty())
	{
		(*materials.begin())->StackTexture();
	}
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

	if (!materials.empty())
	{
		(*materials.begin())->PopTexture();
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glFrontFace(GL_CCW);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void C_Mesh::AddMaterial(C_Material* material)
{
	//TMP: we could have multiple materials in one mesh
	if (materials.empty())
		materials.push_back(material);
}

void C_Mesh::CreateMaterial()
{
	if (materials.empty())
	{
		materials.push_back(new C_Material(gameObject));
	}
}

void C_Mesh::RemoveMaterial(C_Material* material)
{
	materials.remove(material);
}

const C_Material* C_Mesh::GetMaterial(uint position) const
{
	if (materials.empty())
		return NULL;
	std::list<C_Material*>::const_iterator it = materials.begin();
	for (uint i = 0; i < position; i++)
	{
		it++;
	}
	return (*it);
}

uint C_Mesh::GetMaterialsSize() const
{
	return materials.size();
}