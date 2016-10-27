#include "Gizmos.h"

//TODO: templatizate this 3 functions? -----------------------
void Gizmos::DrawWireBox(const AABB& aabb, Color color)
{
	float3 corners[8];
	aabb.GetCornerPoints(corners);
	DrawWireCube(corners, color);
}

void Gizmos::DrawWireBox(const OBB& obb, Color color)
{
	float3 corners[8];
	obb.GetCornerPoints(corners);
	DrawWireCube(corners, color);
}

void Gizmos::DrawWireFrustum(const Frustum& frustum, Color color)
{
	float3 corners[8];
	frustum.GetCornerPoints(corners);
	DrawWireCube(corners, color);
}
//TODO END ---------------------------------------------------

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