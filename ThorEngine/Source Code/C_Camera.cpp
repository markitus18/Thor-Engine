#include "C_Camera.h"
#include "GameObject.h"

#include "Engine.h"
#include "M_Renderer3D.h"
#include "OpenGL.h"

C_Camera::C_Camera(GameObject* gameObject) : Component(Component::Type::Camera, gameObject, false), backgroundColor(0.278f, 0.278f, 0.278f, 1.0f )
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetPos(float3(0, 0, 0));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);

	frustum.SetViewPlaneDistances(0.1f, 1000.0f);
	frustum.SetPerspective(1.0f, 1.0f);

	UpdatePlanes();
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
}

void C_Camera::SetFarPlane(float distance)
{
	if (distance > 0 && distance > frustum.NearPlaneDistance())
		frustum.SetViewPlaneDistances(frustum.NearPlaneDistance(), distance);
	UpdatePlanes();
}

//Setting vertical FOV in degrees 
void C_Camera::SetFOV(float fov)
{
	fov *= DEGTORAD;
	frustum.SetHorizontalFovAndAspectRatio(fov, frustum.AspectRatio());
	UpdatePlanes();
}

void C_Camera::SetAspectRatio(float ar)
{
	float horizontalFov = frustum.HorizontalFov();
	frustum.SetHorizontalFovAndAspectRatio(horizontalFov, ar);
	UpdatePlanes();
}
//--------------------------------

void C_Camera::Look(const float3& position)
{
	float3 vector = position - frustum.Pos();

	float3x3 matrix = float3x3::LookAt(frustum.Front(), vector.Normalized(), frustum.Up(), float3::unitY);

	frustum.SetFront(matrix.MulDir(frustum.Front()).Normalized());
	frustum.SetUp(matrix.MulDir(frustum.Up()).Normalized());
	UpdatePlanes();
}

void C_Camera::Match(const C_Camera* reference)
{
	frustum.SetPos(reference->frustum.Pos());
	frustum.SetFront(reference->frustum.Front());
	frustum.SetUp(reference->frustum.Up());
	UpdatePlanes();
}

void C_Camera::SetRenderingEnabled(bool enabled)
{
	if (renderingEnabled != enabled)
	{
		renderingEnabled = enabled;
		if (gameObject->sceneOwner)
		{
			//TODO: Call OnCameraEnabledChanged
		}
	}
}

void C_Camera::GenerateFrameBuffer()
{

}

void C_Camera::UpdateFrameBuffer()
{

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
}

void C_Camera::Draw(RenderingFlags flags)
{
	if (flags & RenderFlag_Frustum)
		Engine->renderer3D->AddFrustum(frustum, Blue);
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