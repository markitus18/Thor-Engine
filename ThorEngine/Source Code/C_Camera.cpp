#include "C_Camera.h"
#include "GameObject.h"

#include "Engine.h"
#include "M_Renderer3D.h"
#include "OpenGL.h"

#define DEFAULT_CAMERA_WIDTH 1920
#define DEFAULT_CAMERA_HEIGHT 1080

C_Camera::C_Camera(GameObject* gameObject) : Component(Component::Type::Camera, gameObject, false), backgroundColor(0.278f, 0.278f, 0.278f, 1.0f ), resolution(DEFAULT_CAMERA_WIDTH, DEFAULT_CAMERA_HEIGHT)
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(0.1f, 1000.0f);
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * 65.0f, (float)DEFAULT_CAMERA_WIDTH / (float)DEFAULT_CAMERA_HEIGHT);

	frustum.SetPos(float3(10, 10, 10));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);

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
	float verticalFov = frustum.VerticalFov();
	frustum.SetVerticalFovAndAspectRatio(verticalFov, ar);
	UpdatePlanes();
}

void C_Camera::SetResolution(float width, float height)
{
	resolution = float2(width, height);
	UpdateFrameBufferSize();
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
	//Generating buffers for target rendering
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	//Generating texture to render to
	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution.x, resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Generating the depth & stencil buffer
	glGenRenderbuffers(1, &depthStencilBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resolution.x, resolution.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthStencilBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//Configuring frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG("Error creating camera frame buffer");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void C_Camera::UpdateFrameBufferSize()
{
	//Update texture size
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution.x, resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Update depth & stencil buffer size
	glBindRenderbuffer(GL_RENDERBUFFER, depthStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resolution.x, resolution.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

float * C_Camera::GetOpenGLViewMatrix() const
{
	static float4x4 view;
	view = frustum.ViewMatrix();
	view.Transpose();
	return *view.v;
}

float * C_Camera::GetOpenGLProjectionMatrix() const
{
	static float4x4 proj;
	proj = frustum.ProjectionMatrix();
	proj.Transpose();
	return *proj.v;
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

void C_Camera::Draw(RenderingSettings::RenderingFlags flags)
{
	if (flags & RenderingSettings::RenderFlag_Frustum)
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