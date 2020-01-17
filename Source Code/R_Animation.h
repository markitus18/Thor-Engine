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

	bool HasPosKey() const;
	std::map<double, float3>::const_iterator GetPrevPosKey(double currentKey) const;
	std::map<double, float3>::const_iterator GetNextPosKey(double currentKey) const;

	bool HasRotKey() const;
	std::map<double, Quat>::const_iterator GetPrevRotKey(double currentKey) const;
	std::map<double, Quat>::const_iterator GetNextRotKey(double currentKey) const;

	bool HasScaleKey() const;
	std::map<double, float3>::const_iterator GetPrevScaleKey(double currentKey) const;
	std::map<double, float3>::const_iterator GetNextScaleKey(double currentKey) const;
};

class R_Animation : public Resource
{
public:

	R_Animation();
	~R_Animation();

	float duration;
	float ticksPerSecond;
	bool loopable = true;

	std::map<std::string, Channel> channels;
};

#endif
