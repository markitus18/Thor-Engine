#include "Gizmos.h"

//template<typename Box>
//void Gizmos::DrawWireBox(const Box& box, Color color)
//{
//	float3 corners[8];
//	box.GetCornerPoints(corners);
//	DrawWireCube(corners, color);
//}
//This allows us to declarate a templatized function in .cpp file
//template void Gizmos::DrawWireBox<AABB>(const AABB& aabb, Color color);
//template void Gizmos::DrawWireBox<OBB>(const OBB& obb, Color color);
//template void Gizmos::DrawWireBox<Frustum>(const Frustum& frustum, Color color);

void Gizmos::DrawWireCube(const float3* corners, Color color)
{
	glColor4f(color.r, color.g, color.b, color.a);

	//Between-planes right
	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[7]);
	glVertex3fv((GLfloat*)&corners[3]);

	//Between-planes left
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[2]);
	glVertex3fv((GLfloat*)&corners[6]);

	//Far plane horizontal
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[6]);
	glVertex3fv((GLfloat*)&corners[7]);

	//Near plane horizontal
	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[3]);
	glVertex3fv((GLfloat*)&corners[2]);

	//Near plane vertical
	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[3]);
	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[2]);

	//Far plane vertical
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[7]);
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[6]);

	glColor4f(1.0, 1.0, 1.0, 1.0);
}