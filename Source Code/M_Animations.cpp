#include "M_Animations.h"

//#include "R_Mesh.h"
#include "R_Animation.h"

#include "C_Mesh.h"

#include "Assimp/include/scene.h"

void Importer::Animations::Import(const aiAnimation* animation, R_Animation* rAnimation)
{
	//Import general animation data
	rAnimation->duration = animation->mDuration;
	rAnimation->ticksPerSecond = animation->mTicksPerSecond;

	//Import all channels data
	for (uint i = 0; i < animation->mNumChannels; i++)
	{
		std::string nodeName = animation->mChannels[i]->mNodeName.C_Str();

		uint symbolPos = nodeName.find("_$AssimpFbx$_");
		if (symbolPos != std::string::npos)
		{
			nodeName = nodeName.substr(0, symbolPos);
		}

		rAnimation->channels[nodeName] = Channel();
		Private::ImportChannel(animation->mChannels[i], rAnimation->channels[nodeName]);
	}
}

uint64 Importer::Animations::Save(const R_Animation* rAnimation, char** buffer)
{
	//Animation source file (size and string), animation name(size and string), duration, ticks per sec, ranges, numChannels, channels
	uint size = sizeof(float) + sizeof(float) + sizeof(uint);
	std::map<std::string, Channel>::const_iterator it;
	for (it = rAnimation->channels.begin(); it != rAnimation->channels.end(); ++it)
		size += Private::CalcChannelSize(it->second);

	//Allocate buffer size
	*buffer = new char[size];
	char* cursor = *buffer;

	//Duration
	memcpy(cursor, &rAnimation->duration, sizeof(float));
	cursor += sizeof(float);

	//Ticks per sec
	memcpy(cursor, &rAnimation->ticksPerSecond, sizeof(float));
	cursor += sizeof(float);

	//Channels number
	uint channelsSize = rAnimation->channels.size();
	memcpy(cursor, &channelsSize, sizeof(uint));
	cursor += sizeof(uint);
	
	for (it = rAnimation->channels.begin(); it != rAnimation->channels.end(); ++it)
		Private::SaveChannel(it->second, &cursor);

	return size;
}

void Importer::Animations::Load(const char* buffer, R_Animation* rAnimation)
{
	const char* cursor = buffer;
	uint bytes;

	//Duration
	memcpy(&rAnimation->duration, cursor, sizeof(float));
	cursor += sizeof(float);

	//Ticks per sec
	memcpy(&rAnimation->ticksPerSecond, cursor, sizeof(float));
	cursor += sizeof(float);

	//Channels number
	uint channelsSize = 0;
	memcpy(&channelsSize, cursor, sizeof(uint));
	cursor += sizeof(uint);

	for (uint i = 0; i < channelsSize; ++i)
	{
		Channel newChannel;
		Private::LoadChannel(newChannel, &cursor);
		rAnimation->channels[newChannel.name.c_str()] = newChannel;
	}
}

void Importer::Animations::Private::ImportChannel(const aiNodeAnim* node, Channel& channel)
{
	channel.name = node->mNodeName.C_Str();
	uint pos = channel.name.find("_$AssimpFbx$_");
	if (pos != std::string::npos)
	{
		channel.name = channel.name.substr(0, pos);
	}
	//Loading position keys
	for (uint i = 0; i < node->mNumPositionKeys; i++)
		channel.positionKeys[node->mPositionKeys[i].mTime] = float3(node->mPositionKeys[i].mValue.x, node->mPositionKeys[i].mValue.y, node->mPositionKeys[i].mValue.z);

	//Loading rotation keys
	for (uint i = 0; i < node->mNumRotationKeys; i++)
		channel.rotationKeys[node->mRotationKeys[i].mTime] = Quat(node->mRotationKeys[i].mValue.x, node->mRotationKeys[i].mValue.y, node->mRotationKeys[i].mValue.z, node->mRotationKeys[i].mValue.w);

	//Loading scale keys
	for (uint i = 0; i < node->mNumScalingKeys; i++)
		channel.scaleKeys[node->mScalingKeys[i].mTime] = float3(node->mScalingKeys[i].mValue.x, node->mScalingKeys[i].mValue.y, node->mScalingKeys[i].mValue.z);

}

uint Importer::Animations::Private::CalcChannelSize(const Channel& channel)
{
	//Name (size and string) // Ranges (pos, rot, scale) // Pos floats // Pos // Rot floats // Rots // Scale floats // Scales
	uint ret = sizeof(uint) + channel.name.size() + sizeof(uint) * 3;
	//Positions
	ret += sizeof(double) * channel.positionKeys.size() + sizeof(float) * channel.positionKeys.size() * 3;
	//Rotations
	ret += sizeof(double) * channel.rotationKeys.size() + sizeof(float) * channel.rotationKeys.size() * 4;
	//Scales
	ret += sizeof(double) * channel.scaleKeys.size() + sizeof(float) * channel.scaleKeys.size() * 3;

	return ret;
}

void Importer::Animations::Private::SaveChannel(const Channel& channel, char** cursor)
{
	//Name (size and string)
	uint nameSize = channel.name.size();
	memcpy(*cursor, &nameSize, sizeof(uint));
	*cursor += sizeof(uint);

	memcpy(*cursor, channel.name.c_str(), channel.name.size());
	*cursor += channel.name.size();

	//Ranges
	uint ranges[3] = { channel.positionKeys.size(), channel.rotationKeys.size(), channel.scaleKeys.size() };
	memcpy(*cursor, ranges, sizeof(uint) * 3);
	*cursor += sizeof(uint) * 3;

	SaveChannelKeys(channel.positionKeys, cursor);
	SaveChannelKeys(channel.rotationKeys, cursor);
	SaveChannelKeys(channel.scaleKeys, cursor);
}

void Importer::Animations::Private::SaveChannelKeys(const std::map<double, float3>& map, char** cursor)
{
	//Keys save structure: float-float3 // float-float3 // ...
	std::map<double, float3>::const_iterator it = map.begin();
	for (it = map.begin(); it != map.end(); it++)
	{
		memcpy(*cursor, &it->first, sizeof(double));
		*cursor += sizeof(double);

		memcpy(*cursor, &it->second, sizeof(float) * 3);
		*cursor += sizeof(float) * 3;
	}
}


void Importer::Animations::Private::SaveChannelKeys(const std::map<double, Quat>& map, char** cursor)
{
	//Keys save structure: float-float4 // float-float4 // ...
	std::map<double, Quat>::const_iterator it = map.begin();
	for (it = map.begin(); it != map.end(); it++)
	{
		memcpy(*cursor, &it->first, sizeof(double));
		*cursor += sizeof(double);

		memcpy(*cursor, &it->second, sizeof(float) * 4);
		*cursor += sizeof(float) * 4;
	}
}

void Importer::Animations::Private::LoadChannel(Channel& channel, const char** cursor)
{
	uint bytes = 0;

	//Name (size and string)
	uint nameSize = 0;
	memcpy(&nameSize, *cursor, sizeof(uint));
	*cursor += sizeof(uint);

	if (nameSize > 0)
	{
		char* string = new char[nameSize + 1];
		bytes = sizeof(char) * nameSize;
		memcpy(string, *cursor, bytes);
		*cursor += bytes;
		string[nameSize] = '\0';
		channel.name = string;
		RELEASE_ARRAY(string);
	}

	//Ranges
	uint ranges[3];
	memcpy(&ranges, *cursor, sizeof(uint) * 3);
	*cursor += sizeof(uint) * 3;

	LoadChannelKeys(channel.positionKeys, cursor, ranges[0]);
	LoadChannelKeys(channel.rotationKeys, cursor, ranges[1]);
	LoadChannelKeys(channel.scaleKeys, cursor, ranges[2]);

}

void Importer::Animations::Private::LoadChannelKeys(std::map<double, float3>& map, const char** cursor, uint size)
{
	for (uint i = 0; i < size; i++)
	{
		double time;
		memcpy(&time, *cursor, sizeof(double));
		*cursor += sizeof(double);
		float data[3];
		memcpy(&data, *cursor, sizeof(float) * 3);
		*cursor += sizeof(float) * 3;

		map[time] = float3(data);
	}
}

void Importer::Animations::Private::LoadChannelKeys(std::map<double, Quat>& map, const char** cursor, uint size)
{
	for (uint i = 0; i < size; i++)
	{
		double time;
		memcpy(&time, *cursor, sizeof(double));
		*cursor += sizeof(double);
		float data[4];
		memcpy(&data, *cursor, sizeof(float) * 4);
		*cursor += sizeof(float) * 4;

		map[time] = Quat(data);
	}
}