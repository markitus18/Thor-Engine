#include "R_Animation.h"

bool Channel::HasPosKey() const
{
	return ((positionKeys.size() == 1 && positionKeys.begin()->first == -1) == false);
}

std::map<double, float3>::const_iterator Channel::GetPrevPosKey(double currentKey) const
{	
	std::map<double, float3>::const_iterator ret = positionKeys.lower_bound(currentKey);
	if (ret != positionKeys.begin())
		ret--;

	return ret;
}

std::map<double, float3>::const_iterator Channel::GetNextPosKey(double currentKey) const
{
	return positionKeys.upper_bound(currentKey);
}

bool Channel::HasRotKey() const
{
	return ((rotationKeys.size() == 1 && rotationKeys.begin()->first == -1) == false);
}

std::map<double, Quat>::const_iterator Channel::GetPrevRotKey(double currentKey) const
{
	std::map<double, Quat>::const_iterator ret = rotationKeys.lower_bound(currentKey);
	if (ret != rotationKeys.begin())
		ret--;
	return ret;
}

std::map<double, Quat>::const_iterator Channel::GetNextRotKey(double currentKey) const
{
	return rotationKeys.upper_bound(currentKey);
}

bool Channel::HasScaleKey() const
{
	return ((scaleKeys.size() == 1 && scaleKeys.begin()->first == -1) == false);
}

std::map<double, float3>::const_iterator Channel::GetPrevScaleKey(double currentKey) const
{
	std::map<double, float3>::const_iterator ret = scaleKeys.lower_bound(currentKey);
	if (ret != scaleKeys.begin())
		ret--;
	return ret;
}

std::map<double, float3>::const_iterator Channel::GetNextScaleKey(double currentKey) const
{
	return scaleKeys.upper_bound(currentKey);
}

R_Animation::R_Animation() : Resource(Resource::Type::ANIMATION)
{
	isInternal = true;
}

R_Animation::~R_Animation()
{

}