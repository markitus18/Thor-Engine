#include "R_Animation.h"

#pragma region ChannelKeys

bool Channel::HasPosKey() const
{
	return ((positionKeys.size() == 1 && positionKeys.begin()->first == -1) == false);
}

std::map<double, float3>::iterator Channel::GetPrevPosKey(double currentKey, uint start, uint end)
{	
	std::map<double, float3>::iterator ret = positionKeys.lower_bound(currentKey);
	if (ret != positionKeys.begin())
		ret--;

	return ret;
}

std::map<double, float3>::iterator Channel::GetNextPosKey(double currentKey, uint start, uint end)
{
	return positionKeys.upper_bound(currentKey);
}

bool Channel::HasRotKey() const
{
	return ((rotationKeys.size() == 1 && rotationKeys.begin()->first == -1) == false);
}

std::map<double, Quat>::iterator Channel::GetPrevRotKey(double currentKey, uint start, uint end)
{
	std::map<double, Quat>::iterator ret = rotationKeys.lower_bound(currentKey);
	if (ret != rotationKeys.begin())
		ret--;
	return ret;
}

std::map<double, Quat>::iterator Channel::GetNextRotKey(double currentKey, uint start, uint end)
{
	return rotationKeys.upper_bound(currentKey);
}

bool Channel::HasScaleKey() const
{
	return ((scaleKeys.size() == 1 && scaleKeys.begin()->first == -1) == false);
}

std::map<double, float3>::iterator Channel::GetPrevScaleKey(double currentKey, uint start, uint end)
{
	std::map<double, float3>::iterator ret = scaleKeys.lower_bound(currentKey);
	if (ret != scaleKeys.begin())
		ret--;
	return ret;
}

std::map<double, float3>::iterator Channel::GetNextScaleKey(double currentKey, uint start, uint end)
{
	return scaleKeys.upper_bound(currentKey);
}
#pragma endregion

R_Animation::R_Animation() : Resource(Resource::Type::ANIMATION)
{

}

R_Animation::~R_Animation()
{

}