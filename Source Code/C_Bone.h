#ifndef __C_BONE_H__
#define __C_BONE_H__

#include "Component.h"

class C_Bone : public Component
{
public:
	C_Bone(GameObject* new_GameObject = nullptr);
	~C_Bone();

	float4x4 GetSystemTransform();
	C_Bone* GetRoot();

	static inline Type GetType() { return Type::Bone; };
};

#endif
