#include "C_Camera.h"

#include "GameObject.h"
#include "C_Transform.h"

#include "Engine.h"
#include "M_Renderer3D.h"
#include "OpenGL.h"
#include "Scene.h"

#define DEFAULT_CAMERA_WIDTH 1920
#define DEFAULT_CAMERA_HEIGHT 1080

C_Camera::C_Camera(GameObject* gameObject) : Component(Component::Type::Camera, gameObject, false), backgroundColor(0.278f, 0.278f, 0.278f, 1.0f ), resolution(DEFAULT_CAMERA_WIDTH, DEFAULT_CAMERA_HEIGHT)
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(0.1f, 1000.0f);

	hFov = 65.0f;
	frustum.SetHorizontalFovAndAspectRatio(hFov * DEGTORAD, (float)DEFAULT_CAMERA_WIDTH / (float)DEFAULT_CAMERA_HEIGHT);

	frustum.SetPos(float3(0, 0, 0));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);

	frustum.GetPlanes(planes);
	GenerateFrameBuffer();
}

C_Camera::~C_Camera()
{

}

void C_Camera::SetNearPlane(float distance)
{
	if (distance > 0 && distance < frustum.FarPlaneDistance())
		frustum.SetViewPlaneDistances(distance, frustum.FarPlaneDistance());
	frustum.GetPlanes(planes);
}

void C_Camera::SetFarPlane(float distance)
{
	if (distance > 0 && distance > frustum.NearPlaneDistance())
		frustum.SetViewPlaneDistances(frustum.NearPlaneDistance(), distance);
	frustum.GetPlanes(planes);
}

//Setting vertical FOV in degrees 
void C_Camera::SetFOV(float fov)
{
	hFov = fov;
	frustum.SetHorizontalFovAndAspectRatio(hFov * DEGTORAD, frustum.AspectRatio());
	frustum.GetPlanes(planes);
}

// Set Aspect Ratio has been disabled temporarily, as we cannot set the resolution of the orthographic frustum only from the aspect ratio
/*
void C_Camera::SetAspectRatio(float ar)
{
	if (cameraAngle & EViewportCameraAngle::Perspective)
	{
		float verticalFov = frustum.VerticalFov();
		frustum.SetVerticalFovAndAspectRatio(verticalFov, ar);
	}
	else
	{
		frustum.SetOrthographic()
	}

	frustum.GetPlanes(planes);
}
*/

void C_Camera::SetSize(float newSize)
{
	size = Clamp(newSize, 1.0f, 20.0f);
	SetResolution(resolution.x, resolution.y);
}

void C_Camera::SetResolution(float width, float height)
{
	resolution = float2(width, height);
	if (cameraAngle & EViewportCameraAngle::Perspective)
	{
		float verticalFov = frustum.VerticalFov();
		frustum.SetVerticalFovAndAspectRatio(verticalFov, width / height);
	}
	else
	{
		frustum.SetOrthographic(width / size, height / size);
	}
	UpdateFrameBufferSize();
}

void C_Camera::SetCameraAngle(EViewportCameraAngle::Flags newCameraAngle)
{
	cameraAngle = newCameraAngle;
	if (cameraAngle & EViewportCameraAngle::Perspective)
	{
		SetFOV(hFov);
	}
	else
	{
		frustum.SetOrthographic(resolution.x, resolution.y);
		frustum.GetPlanes(planes);
	}
}
//--------------------------------

LineSegment C_Camera::GenerateRaycast(float normalizedX, float normalizedY)
{
	return lastRay = frustum.UnProjectLineSegment(normalizedX, normalizedY);
}

void C_Camera::SetRenderingEnabled(bool enabled)
{
	if (renderingEnabled != enabled)
	{
		renderingEnabled = enabled;
		if (gameObject->sceneOwner)
		{
			renderingEnabled ? gameObject->sceneOwner->RegisterCamera(this) : gameObject->sceneOwner->UnregisterCamera(this);
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

float4x4 C_Camera::GetOpenGLViewMatrix() const
{
	static float4x4 view;
	view = frustum.ViewMatrix();
	view.Transpose();
	return view;
}

float4x4 C_Camera::GetOpenGLProjectionMatrix() const
{
	static float4x4 proj;
	proj = frustum.ProjectionMatrix();
	proj.Transpose();
	return proj;
}

void C_Camera::OnTransformUpdated()
{
	C_Transform* transform = gameObject->GetComponent<C_Transform>();
	frustum.SetFrame(transform->GetPosition(), transform->GetFwd(), transform->GetUp());
	frustum.GetPlanes(planes);
}

void C_Camera::Draw(ERenderingFlags::Flags flags)
{
	if (flags & ERenderingFlags::Frustum)
		Engine->renderer3D->AddFrustum(frustum, Blue);

	if (flags & ERenderingFlags::MousePick)
		Engine->renderer3D->AddLine(lastRay.a, lastRay.b, Red);
}

void C_Camera::Serialize(Config& config)
{
	Component::Serialize(config);

	float currentHFov = frustum.HorizontalFov() * RADTODEG;
	config.Serialize("FOV", currentHFov);
	
	float nearPlane = frustum.NearPlaneDistance();
	config.Serialize("Near Plane", nearPlane);

	float farPlane = frustum.NearPlaneDistance();
	config.Serialize("Far Plane", farPlane);

	if (!config.isSaving)
	{
		SetFOV(currentHFov);
		SetNearPlane(nearPlane);
		SetFarPlane(farPlane);
	}
}