#include "C_Mesh.h"
#include "OpenGL.h"
#include "GameObject.h"

C_Mesh::C_Mesh(GameObject* new_GameObject) : Component(Type::Mesh, new_GameObject)
{

}

C_Mesh::~C_Mesh()
{
	for (uint i = 0; i < materials.size(); i++)
	{
		if (materials[i] != NULL)
		{
			delete materials[i];
			materials[i] = NULL;
		}
	}
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

void C_Mesh::Draw(bool shaded, bool wireframe)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	//Removed temporaly: game objects will use buffers, not mesh itself
	
	bool selected = gameObject->IsSelected();
	bool parentSelected = gameObject->IsParentSelected();

	if (selected || parentSelected || wireframe)
	{
		if (selected)
		{
			glColor3f(0.71, 0.78, 0.88);
			glLineWidth(1);
		}

		else if (parentSelected)
		{
			glColor3f(0.51, 0.58, 0.68);
		}
		
		else if (wireframe)
		{
			glColor3f(0, 0, 0);
		}

		glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_LIGHTING);
		glLineWidth(1);
		glColor4f(1, 1, 1, 1);
	}
	
	if (shaded)
	{
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
	}

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void C_Mesh::AddMaterial(C_Material* material)
{
	//TMP: we could have multiple materials in one mesh
	if (materials.empty())
		materials.push_back(material);
}

Component* C_Mesh::CreateMaterial()
{
	Component* component = NULL;
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
		return NULL;

	return materials[position];
}

uint C_Mesh::GetMaterialsSize() const
{
	return materials.size();
}