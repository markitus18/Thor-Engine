#include "C_Billboard.h"

#include "GameObject.h"
#include "Scene.h"

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
		gameObject->sceneOwner->GetMainCamera();
	}
	if (cameraRef == nullptr) return;
	
	float3 right, up, fwd;
	C_Transform& transform = *gameObject->GetComponent<C_Transform>();
	C_Transform& cameraTransform = *cameraRef->gameObject->GetComponent<C_Transform>();
	GetVectorsFromAlignment(transform, cameraTransform, right, up, fwd);

	//TODO: Send transform to shader, calculate there based on view matrix
	/* Read line 140 from float4x4 to understand function usage */
	float4x4 resultMatrix(right.ToDir4(), up.ToDir4(), fwd.ToDir4(), transform.GetPosition().ToPos4());
	transform.SetGlobalTransform(resultMatrix);
}

void C_Billboard::GetVectorsFromAlignment(const C_Transform& transform, const C_Transform& cameraTransform, float3&right, float3&up, float3& fwd) const
{
	switch (alignment)
	{
		case(Alignment::Screen_Aligned):
			fwd = cameraTransform.GetFwd().Neg();
			up = cameraTransform.GetUp();
			right = up.Cross(fwd).Normalized();
			break;
		case(Alignment::Camera_Aligned):
			fwd = float3(cameraTransform.GetPosition() - transform.GetPosition()).Normalized();
			right = cameraTransform.GetUp().Cross(fwd).Normalized();
			up = fwd.Cross(right).Normalized();
			break;
		case(Alignment::Axis_Aligned):
			float3 direction = float3(cameraTransform.GetPosition() - transform.GetPosition()).Normalized();
			switch (lockAxis)
			{
				//TODO: same as Unreal, this could be xy, xz, yx, yz, zx, zy. Here we are taking arbitrary decisions on which is the "forward" vector
				case(Axis::x):
					right = float3::unitX;
					up = direction.Cross(right).Normalized();
					fwd = right.Cross(up).Normalized();
					break;
				case(Axis::y):
					up = float3::unitY;
					right = up.Cross(direction).Normalized();
					fwd = right.Cross(up).Normalized();
					break;
				case(Axis::z):
					fwd = float3::unitZ;
					right = direction.Cross(fwd).Normalized();
					up = fwd.Cross(right).Normalized();
					break;
			}
			break;
	}
}