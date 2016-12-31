#ifndef __R_ANIMATION_H__
#define __R_ANIMATION_H__

#include "Resource.h"
#include "MathGeoLib\src\MathGeoLib.h"
#include <map>

struct Channel
{
	std::string name;

	std::map<double, float3> positionKeys;
	std::map<double, Quat> rotationKeys;
	std::map<double, float3> scaleKeys;

	std::map<double, float3>::iterator GetPrevPosKey(std::map<double, float3>::iterator lastPrev, double currentKey);
	std::map<double, float3>::iterator GetNextPosKey(std::map<double, float3>::iterator previous, double currentKey);

	std::map<double, Quat>::iterator GetPrevRotKey(std::map<double, Quat>::iterator lastPrev, double currentKey);
	std::map<double, Quat>::iterator GetNextRotKey(std::map<double, Quat>::iterator previous, double currentKey);

	std::map<double, float3>::iterator GetPrevScaleKey(std::map<double, float3>::iterator lastPrev, double currentKey);
	std::map<double, float3>::iterator GetNextScaleKey(std::map<double, float3>::iterator previous, double currentKey);
};

class R_Animation : public Resource
{
public:

	R_Animation();
	~R_Animation();

	float duration;
	float ticksPerSecond;
	bool loopable = true;
	uint numChannels;

	Channel* channels;
};

#endif
