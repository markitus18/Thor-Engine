#include "R_Animation.h"

#pragma region ChannelKeys

bool Channel::HasPosKey() const
{
	return ((positionKeys.size() == 1 && positionKeys.begin()->first == -1) == false);
}

std::map<double, float3>::iterator Channel::GetPrevPosKey(std::map<double, float3>::iterator lastPrev, double currentKey, uint start, uint end)
{	
	//Looping through keys: start when key frame is equal or bigger than "start" frame
	if (lastPrev->first > currentKey)
	{
		lastPrev = positionKeys.begin();
		std::map<double, float3>::iterator it = positionKeys.begin();
		for (it = positionKeys.begin(); it != positionKeys.end(); it++)
		{
			if (it->first >= start)
			{
				lastPrev = it;
				break;
			}
		}
	}


	std::map<double, float3>::iterator it = lastPrev;
	it++;
	while (it != positionKeys.end() && it->first <= currentKey)
	{
		lastPrev = it;
		it++;
	}
	return lastPrev;
}

std::map<double, float3>::iterator Channel::GetNextPosKey(std::map<double, float3>::iterator previous, double currentKey, uint start, uint end)
{
	if (previous->first > currentKey)
		return previous;
	else
	{
		std::map<double, float3>::iterator it = previous;
		it++;
		if (it != positionKeys.end() && (end == 0 ? true : it->first < end))
			return it;
		else
			return previous;
	}
}

bool Channel::HasRotKey() const
{
	return ((rotationKeys.size() == 1 && rotationKeys.begin()->first == -1) == false);
}

std::map<double, Quat>::iterator Channel::GetPrevRotKey(std::map<double, Quat>::iterator lastPrev, double currentKey, uint start, uint end)
{
	//Looping through keys
	if (lastPrev->first > currentKey)
	{
		lastPrev = rotationKeys.begin();
		std::map<double, Quat>::iterator it = rotationKeys.begin();
		for (it = rotationKeys.begin(); it != rotationKeys.end(); it++)
		{
			if (it->first >= start)
			{
				lastPrev = it;
				break;
			}
		}
	}


	std::map<double, Quat>::iterator it = lastPrev;
	it++;
	while (it != rotationKeys.end() && it->first <= currentKey)
	{
		lastPrev = it;
		it++;
	}
	return lastPrev;
}

std::map<double, Quat>::iterator Channel::GetNextRotKey(std::map<double, Quat>::iterator previous, double currentKey, uint start, uint end)
{
	if (previous->first > currentKey)
		return previous;
	else
	{
		std::map<double, Quat>::iterator it = previous;
		it++;
		if (it != rotationKeys.end() && (end == 0 ? true : it->first <= end))
			return it;
		else
			return previous;
	}
}

bool Channel::HasScaleKey() const
{
	return ((scaleKeys.size() == 1 && scaleKeys.begin()->first == -1) == false);
}

std::map<double, float3>::iterator Channel::GetPrevScaleKey(std::map<double, float3>::iterator lastPrev, double currentKey, uint start, uint end)
{
	//Looping through keys
	if (lastPrev->first > currentKey)
	{
		lastPrev = scaleKeys.begin();
		std::map<double, float3>::iterator it = scaleKeys.begin();
		for (it = scaleKeys.begin(); it != scaleKeys.end(); it++)
		{
			if (it->first >= start)
			{
				lastPrev = it;
				break;
			}
		}
	}

	std::map<double, float3>::iterator it = lastPrev;
	it++;
	while (it != scaleKeys.end() && it->first <= currentKey)
	{
		lastPrev = it;
		it++;
	}
	return lastPrev;
}

std::map<double, float3>::iterator Channel::GetNextScaleKey(std::map<double, float3>::iterator previous, double currentKey, uint start, uint end)
{
	if (previous->first > currentKey)
		return previous;
	else
	{
		std::map<double, float3>::iterator it = previous;
		it++;
		if (it != scaleKeys.end() && (end == 0 ? true : it->first < end))
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