#include "R_Mesh.h"


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