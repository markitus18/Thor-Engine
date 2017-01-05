#ifndef __R_BONE_H__
#define __R_BONE_H__

#include "Resource.h"
#include "MathGeoLib\src\Math\float4x4.h"

class R_Bone : public Resource
{
public:
	R_Bone();
	~R_Bone();

	uint numWeights;
	uint* weightsIndex;
	float* weights;
	float4x4 offset;

	uint64 meshID;
};

#endif __R_BONE_H__