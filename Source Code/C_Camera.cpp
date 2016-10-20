#include "C_Camera.h"
#include "GameObject.h"

C_Camera::C_Camera(GameObject* gameObject) : Component(Component::Type::Camera, gameObject)
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetPos(float3(10, 50, 0));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);

	frustum.SetViewPlaneDistances(0.1f, 1000.0f);
	frustum.SetPerspective(1.0f, 1.0f);

	update_projection = true;
}
C_Camera::~C_Camera()
{

}

//Frustum variable management ----
float C_Camera::GetNearPlane() const
{
	//return frustum.nearPlaneDistance;
	return 0;
}

float C_Camera::GetFarPlane() const
{
	//return frustum.farPlaneDistance;
	return 0;
}

//Returns FOV in degrees
float C_Camera::GetFOV() const
{
	//return frustum.verticalFov * RADTODEG;
	return 0;
}

float C_Camera::GetAspectRatio() const
{
	return frustum.AspectRatio();
}

void C_Camera::SetNearPlane(float distance)
{
	//if (distance > 0 && distance < frustum.farPlaneDistance)
	//	frustum.nearPlaneDistance = distance;
}

void C_Camera::SetFarPlane(float distance)
{
	//if (distance > 0 && distance > frustum.nearPlaneDistance)
	//	frustum.nearPlaneDistance = distance;
}

void C_Camera::SetFOV(float fov)
{
	//float ar = frustum.AspectRatio();
	//frustum.verticalFov = fov;
	//SetAspectRatio(ar);
}

void C_Camera::SetAspectRatio(float ar)
{
	//frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * ar);
}
//--------------------------------

void C_Camera::Look(const float3& position)
{
	//float3 vector = position - frustum.pos;

	//float3x3 matrix = float3x3::LookAt(frustum.front, vector.Normalized(), frustum.up, float3::unitY);

	//frustum.front = matrix.MulDir(frustum.front).Normalized();
	//frustum.up = matrix.MulDir(frustum.up).Normalized();
}

float * C_Camera::GetOpenGLViewMatrix()
{
	static float4x4 m;

	m = frustum.ViewMatrix();
	m.Transpose();

	return (float*)m.v;
}

float * C_Camera::GetOpenGLProjectionMatrix()
{
	static float4x4 m;

	m = frustum.ProjectionMatrix();
	m.Transpose();

	return (float*)m.v;
}
