#ifndef __R_ANIMATION_H__
#define __R_ANIMATION_H__

#include "Resource.h"
#include "MathGeoLib\src\MathGeoLib.h"
#include <map>

struct Channel
{
	uint numPositionKeys = 0;
	uint numScalingKeys = 0;
	uint numRotationKeys = 0;

	std::map<double, float3> positionKeys;
	std::map<double, Quat> rotationKeys;
	std::map<double, float3> scaleKeys;
};

class R_Animation : public Resource
{
public:

	R_Animation();
	~R_Animation();

	float duration;
	float ticksPerSecond;
	uint numChannels;

	Channel* channel;
};

#endif
