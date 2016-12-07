#include "C_Camera.h"
#include "GameObject.h"

C_Camera::C_Camera(GameObject* gameObject) : Component(Component::Type::Camera, gameObject, false)
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetPos(float3(0, 0, 0));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);

	frustum.SetViewPlaneDistances(0.1f, 1000.0f);
	frustum.SetPerspective(1.0f, 1.0f);

	UpdatePlanes();
	update_projection = true;
}
C_Camera::~C_Camera()
{

}

//Frustum variable management ----
float C_Camera::GetNearPlane() const
{
	return frustum.NearPlaneDistance();
}

float C_Camera::GetFarPlane() const
{
	return frustum.FarPlaneDistance();
}

//Returns FOV in degrees
float C_Camera::GetFOV() const
{
	return frustum.VerticalFov() * RADTODEG;
}

float C_Camera::GetAspectRatio() const
{
	return frustum.AspectRatio();
}

void C_Camera::SetNearPlane(float distance)
{
	if (distance > 0 && distance < frustum.FarPlaneDistance())
		frustum.SetViewPlaneDistances(distance, frustum.FarPlaneDistance());
	UpdatePlanes();
	update_projection = true;
}

void C_Camera::SetFarPlane(float distance)
{
	if (distance > 0 && distance > frustum.NearPlaneDistance())
		frustum.SetViewPlaneDistances(frustum.NearPlaneDistance(), distance);
	UpdatePlanes();
	update_projection = true;
}

//Setting vertical FOV in degrees 
void C_Camera::SetFOV(float fov)
{
	fov *= DEGTORAD;
	frustum.SetVerticalFovAndAspectRatio(fov, frustum.AspectRatio());
	UpdatePlanes();
	update_projection = true;
}

void C_Camera::SetAspectRatio(float ar)
{
	float horizontalFov = frustum.HorizontalFov();
	frustum.SetHorizontalFovAndAspectRatio(horizontalFov, ar);
	UpdatePlanes();
	update_projection = true;
}
//--------------------------------

void C_Camera::Look(const float3& position)
{
	float3 vector = position - frustum.Pos();

	float3x3 matrix = float3x3::LookAt(frustum.Front(), vector.Normalized(), frustum.Up(), float3::unitY);

	frustum.SetFront(matrix.MulDir(frustum.Front()).Normalized());
	frustum.SetUp(matrix.MulDir(frustum.Up()).Normalized());
	UpdatePlanes();
	update_projection = true;
}

void C_Camera::Match(const C_Camera* reference)
{
	frustum.SetPos(reference->frustum.Pos());
	frustum.SetFront(reference->frustum.Front());
	frustum.SetUp(reference->frustum.Up());
	UpdatePlanes();
	update_projection = true;
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

void C_Camera::OnUpdateTransform(const float4x4& global, const float4x4& parent_global)
{
	frustum.SetFront(global.WorldZ());
	frustum.SetUp(global.WorldY());

	float3 position = float3::zero;
	float3 scale = float3::one;
	Quat quat = Quat::identity;
	global.Decompose(position, quat, scale);

	frustum.SetPos(position);
	UpdatePlanes();

	update_projection = true;
}

void C_Camera::Save()
{

}

void C_Camera::Load()
{

}

void C_Camera::UpdatePlanes()
{
	frustum.GetPlanes(planes);
}

Component::Type C_Camera::GetType()
{
	return Component::Type::Camera;
}
