#include "C_Billboard.h"

#include "Application.h"
#include "M_Scene.h"

#include "GameObject.h"

#include "C_Transform.h"
#include "C_Camera.h"

C_Billboard::C_Billboard(GameObject* gameObject) : Component(Type::Billboard, gameObject, false)
{

}

C_Billboard::~C_Billboard()
{

}

void C_Billboard::Update(float dt)
{
	//TODO: enough for now, change it to assign a camera in the inspector
	if (cameraRef == nullptr)
	{
		cameraRef = App->scene->GetMainCamera();
	}
	if (cameraRef == nullptr) return;
	
	if (cameraRef->gameObject->GetComponent<C_Transform>()->GetGlobalPosition().x != lastCameraPosition.x ||
		lastPosition.x != gameObject->GetComponent<C_Transform>()->GetGlobalPosition().x)

	switch (alignment)
	{
		case(Alignment::Screen_Aligned):
			AlignToScreen();
			break;
		case(Alignment::Camera_Aligned):
			AlignToCamera();
			break;
		case(Alignment::Axis_Aligned):
			AlignToAxis();
			break;
	}
}

void C_Billboard::AlignToScreen()
{
	C_Transform* transform = gameObject->GetComponent<C_Transform>();
	C_Transform* cameraTransform = cameraRef->gameObject->GetComponent<C_Transform>();
	float4x4 cameraMatrix = cameraTransform->GetGlobalTransform();

	//Using camera's inverted forward as our new forward vector
	float3 forward = cameraMatrix.WorldZ().Normalized().Neg();
	float3 up = cameraMatrix.WorldY().Normalized();
	float3 right = up.Cross(forward).Normalized();
		//forward.Cross(up).Normalized();

	/* Read line 140 from float4x4 to understand function usage */
	float4x4 resultMatrix(right.ToDir4(), up.ToDir4(), forward.ToDir4(), transform->GetGlobalPosition().ToPos4());
	transform->SetGlobalTransform(resultMatrix);
}

void C_Billboard::AlignToCamera()
{
	C_Transform* transform = gameObject->GetComponent<C_Transform>();
	C_Transform* cameraTransform = cameraRef->gameObject->GetComponent<C_Transform>();
	float4x4 cameraMatrix = cameraTransform->GetGlobalTransform();
	
	//Our new forward vector is the direction from the billboard to the camera
	float3 forward = float3(cameraTransform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
	float3 right = cameraMatrix.WorldY().Normalized().Cross(forward).Normalized();
	float3 up = forward.Cross(right).Normalized();

	/* Read line 140 from float4x4 to understand function usage*/
	float4x4 resultMatrix(right.ToDir4(), up.ToDir4(), forward.ToDir4(), transform->GetGlobalPosition().ToPos4());
	transform->SetGlobalTransform(resultMatrix);
}

void C_Billboard::AlignToAxis()
{
	C_Transform* transform = gameObject->GetComponent<C_Transform>();
	C_Transform* cameraTransform = cameraRef->gameObject->GetComponent<C_Transform>();
	float4x4 cameraMatrix = cameraTransform->GetGlobalTransform();

	//TODO: is there a way to find a generic method for all 3 axis?
	float3 forward, right, up;
	switch (lockAxis)
	{
	case(Axis::x):
	{

		break;
	}
	case(Axis::y):
	{
		up = float3::unitY;
		float3 direction = float3(cameraTransform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
		right = up.Cross(direction).Normalized();
		forward = right.Cross(up).Normalized();
		break;
	}
	case(Axis::z):
	{
		break;
	}
	}

	/* Read line 140 from float4x4 to understand function usage*/
	float4x4 resultMatrix(right.ToDir4(), up.ToDir4(), forward.ToDir4(), transform->GetGlobalPosition().ToPos4());
	transform->SetGlobalTransform(resultMatrix);
}