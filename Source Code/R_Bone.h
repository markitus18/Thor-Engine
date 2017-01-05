#ifndef __R_BONE_H__
#define __R_BONE_H__

#include "Resource.h"
#include "MathGeoLib\src\Math\float4x4.h"

class R_Bone : public Resource
{
public:
	R_Bone();
	~R_Bone();

	uint numWeights = 0;
	uint* weightsIndex = nullptr;
	float* weights = nullptr;
	float4x4 offset = float4x4::identity;

	uint64 meshID = 0;
};

#endif __R_BONE_H__