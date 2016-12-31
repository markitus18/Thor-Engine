#include "R_Animation.h"

#pragma region ChannelKeys
std::map<double, float3>::iterator Channel::GetPrevPosKey(std::map<double, float3>::iterator lastPrev, double currentKey)
{	
	//Looping through keys
	if (lastPrev->first > currentKey)
		lastPrev = positionKeys.begin();

	std::map<double, float3>::iterator it = lastPrev;
	it++;
	while (it != positionKeys.end() && it->first <= currentKey)
	{
		lastPrev = it;
		it++;
	}
	return lastPrev;
}

std::map<double, float3>::iterator Channel::GetNextPosKey(std::map<double, float3>::iterator previous, double currentKey)
{
	if (previous->first > currentKey)
		return previous;
	else
	{
		std::map<double, float3>::iterator it = previous;
		it++;
		if (it != positionKeys.end())
			return it;
		else
			return previous;
	}
}

std::map<double, Quat>::iterator Channel::GetPrevRotKey(std::map<double, Quat>::iterator lastPrev, double currentKey)
{
	//Looping through keys
	if (lastPrev->first > currentKey)
		lastPrev = rotationKeys.begin();

	std::map<double, Quat>::iterator it = lastPrev;
	it++;
	while (it != rotationKeys.end() && it->first <= currentKey)
	{
		lastPrev = it;
		it++;
	}
	return lastPrev;
}

std::map<double, Quat>::iterator Channel::GetNextRotKey(std::map<double, Quat>::iterator previous, double currentKey)
{
	if (previous->first > currentKey)
		return previous;
	else
	{
		std::map<double, Quat>::iterator it = previous;
		it++;
		if (it != rotationKeys.end())
			return it;
		else
			return previous;
	}
}

std::map<double, float3>::iterator Channel::GetPrevScaleKey(std::map<double, float3>::iterator lastPrev, double currentKey)
{
	//Looping through keys
	if (lastPrev->first > currentKey)
		lastPrev = scaleKeys.begin();

	std::map<double, float3>::iterator it = lastPrev;
	it++;
	while (it != scaleKeys.end() && it->first <= currentKey)
	{
		lastPrev = it;
		it++;
	}
	return lastPrev;
}

std::map<double, float3>::iterator Channel::GetNextScaleKey(std::map<double, float3>::iterator previous, double currentKey)
{
	if (previous->first > currentKey)
		return previous;
	else
	{
		std::map<double, float3>::iterator it = previous;
		it++;
		if (it != scaleKeys.end())
			return it;
		else
			return previous;
	}
}
#pragma endregion

R_Animation::R_Animation() : Resource(Resource::Type::ANIMATION)
{

}

R_Animation::~R_Animation()
{

}