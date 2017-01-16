#include "C_Bone.h"
#include "GameObject.h"
#include "MathGeoLib\src\MathGeoLib.h"

C_Bone::C_Bone(GameObject* new_GameObject) : Component(Component::Bone, new_GameObject, true)
{

}

C_Bone::~C_Bone()
{

}

float4x4 C_Bone::GetSystemTransform()
{
	float4x4 transform = gameObject->GetComponent<C_Transform>()->GetGlobalTransform();
	return GetRoot()->gameObject->parent->parent->GetComponent<C_Transform>()->GetGlobalTransform().Inverted() * transform;
}

C_Bone* C_Bone::GetRoot()
{
	C_Bone* parentBone = gameObject->parent->GetComponent<C_Bone>();
	return parentBone == nullptr ? this : parentBone->GetRoot();
}

Component::Type C_Bone::GetType()
{
	return Component::Type::Bone;
}