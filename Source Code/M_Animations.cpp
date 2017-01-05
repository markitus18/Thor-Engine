#include "M_Animations.h"
#include "Application.h"
#include "M_Resources.h"
#include "GameObject.h"
#include "M_FileSystem.h"

#include "Resource.h"
#include "R_Animation.h"
#include "R_Bone.h"

#include "Assimp/include/scene.h"

M_Animations::M_Animations(bool start_enabled) : Module("AnimationImporter", start_enabled)
{

}
M_Animations::~M_Animations()
{

}

uint64 M_Animations::ImportSceneAnimations(const aiScene* scene, GameObject* root, const char* source_file)
{
	uint ret = 0;
	if (scene->HasAnimations() == true)
	{
		std::map<std::string, GameObject*> map;
		CollectGameObjectNames(root, map);

		for (uint i = 0; i < scene->mNumAnimations; i++)
		{
			ret = App->moduleResources->ImportRAnimation(scene->mAnimations[i], source_file, scene->mAnimations[i]->mName.C_Str());
		}
	}
	return ret;
}

R_Animation* M_Animations::ImportAnimation(const aiAnimation* anim, uint64 ID, const char* source_file)
{
	R_Animation* animation = new R_Animation();

	
	//Animation data
	animation->duration = anim->mDuration;
	animation->ticksPerSecond = anim->mTicksPerSecond;
	animation->numChannels = anim->mNumChannels;
	animation->channels = new Channel[animation->numChannels];

	for (uint i = 0; i < anim->mNumChannels; i++)
		LoadChannel(anim->mChannels[i], animation->channels[i]);

	animation->ID = ID;
	animation->name = anim->mName.C_Str();
	std::string full_path("/Library/Animations/");
	full_path.append(std::to_string(ID));
	animation->resource_file = full_path;
	animation->original_file = source_file;
	animation->LoadOnMemory();

	SaveAnimationResource(animation);
	return animation;
}

bool M_Animations::SaveAnimationResource(R_Animation* animation)
{
	//Animation source file (size and string), animation name(size and string), duration, ticks per sec, ranges, numChannels, channels
	uint size = sizeof(uint) * 2 + animation->original_file.size() + animation->name.size() + sizeof(float) + sizeof(float) + sizeof(uint);
	for (uint i = 0; i < animation->numChannels; i++)
		size += CalcChannelSize(animation->channels[i]);

	//Allocate buffer size
	char* data = new char[size];
	char* cursor = data;

	//Store source file and name: Sizes
	uint fileNameSize = animation->original_file.size();
	memcpy(cursor, &fileNameSize, sizeof(uint));
	cursor += sizeof(uint);

	uint nameSize = animation->name.size();
	memcpy(cursor, &nameSize, sizeof(uint));
	cursor += sizeof(uint);

	//Store source file and name: Strings
	memcpy(cursor, animation->original_file.c_str(), animation->original_file.size());
	cursor += animation->original_file.size();

	memcpy(cursor, animation->name.c_str(), animation->name.size());
	cursor += animation->name.size();

	//Duration
	memcpy(cursor, &animation->duration, sizeof(float));
	cursor += sizeof(float);

	//Ticks per sec
	memcpy(cursor, &animation->ticksPerSecond, sizeof(float));
	cursor += sizeof(float);

	//Channels number
	memcpy(cursor, &animation->numChannels, sizeof(uint));
	cursor += sizeof(uint);

	for (uint i = 0; i < animation->numChannels; i++)
		SaveChannelData(animation->channels[i], &cursor);

	uint ret = App->fileSystem->Save(animation->resource_file.c_str(), data, size);
	
	RELEASE_ARRAY(data);
	return ret > 0;
}

R_Animation* M_Animations::LoadAnimation(uint64 ID)
{
	std::string full_path = "/Library/Animations/";
	full_path.append(std::to_string(ID));

	char* buffer;
	uint size = App->fileSystem->Load(full_path.c_str(), &buffer);

	if (size > 0)
	{
		char* cursor = buffer;
		R_Animation* animation = new R_Animation();
		uint bytes;

		uint fileNameSize = 0;
		memcpy(&fileNameSize, cursor, sizeof(uint));
		cursor += sizeof(uint);

		uint nameSize = 0;
		memcpy(&nameSize, cursor, sizeof(uint));
		cursor += sizeof(uint);

		if (fileNameSize > 0)
		{
			char* string = new char[fileNameSize + 1];
			bytes = sizeof(char) * fileNameSize;
			memcpy(string, cursor, bytes);
			cursor += bytes;
			string[fileNameSize] = '\0';
			animation->original_file = string;
			RELEASE_ARRAY(string);
		}

		if (nameSize > 0)
		{
			char* string = new char[nameSize + 1];
			bytes = sizeof(char) * nameSize;
			memcpy(string, cursor, bytes);
			cursor += bytes;
			string[nameSize] = '\0';
			animation->name = string;
			RELEASE_ARRAY(string);
		}

		//Duration
		memcpy(&animation->duration, cursor, sizeof(float));
		cursor += sizeof(float);

		//Ticks per sec
		memcpy(&animation->ticksPerSecond, cursor, sizeof(float));
		cursor += sizeof(float);

		//Channels number
		memcpy(&animation->numChannels, cursor, sizeof(uint));
		cursor += sizeof(uint);

		animation->channels = new Channel[animation->numChannels];
		for (uint i = 0; i < animation->numChannels; i++)
		{
			LoadChannelData(animation->channels[i], &cursor);
		}
		animation->ID = ID;
		animation->resource_file = full_path;

		RELEASE_ARRAY(buffer);
		return animation;
	}
	else
	{
		return nullptr;
	}
}

void M_Animations::ImportSceneBones(const std::vector<const aiMesh*>& bonedMeshes, const std::vector<const GameObject*>& meshGameObjects, GameObject* root, const char* source_file)
{
	std::map<std::string, GameObject*> map;
	CollectGameObjectNames(root, map);

	for (uint i = 0; i < bonedMeshes.size(); i++)
	{
		for (uint b = 0; b < bonedMeshes[i]->mNumBones; b++)
		{
			uint meshID = 0;
			const C_Mesh* mesh = meshGameObjects[i]->GetComponent<C_Mesh>();
			if (mesh != nullptr)
			{
				meshID = mesh->GetResource()->GetID();
			}

			uint64 ID = App->moduleResources->ImportRBone(bonedMeshes[i]->mBones[b], source_file, bonedMeshes[i]->mBones[b]->mName.C_Str(), meshID);
			std::map<std::string, GameObject*>::iterator bone_it = map.find(bonedMeshes[i]->mBones[b]->mName.C_Str());
			if (bone_it != map.end())
			{
				C_Bone* bone = (C_Bone*)bone_it->second->CreateComponent(Component::Type::Bone);
				bone->SetResource(ID);
			}
		}
	}
}

R_Bone* M_Animations::ImportBone(const aiBone* bone, uint64 ID, const char* source_file, uint64 meshID)
{
	R_Bone* rBone = new R_Bone();
	rBone->numWeights = bone->mNumWeights;
	rBone->weights = new float[rBone->numWeights];
	rBone->weightsIndex = new uint[rBone->numWeights];
	rBone->meshID = meshID;
	//TODO: import bone offset matrix
	rBone->offset = float4x4(bone->mOffsetMatrix.a1, bone->mOffsetMatrix.a2, bone->mOffsetMatrix.a3, bone->mOffsetMatrix.a4,
							bone->mOffsetMatrix.b1, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.b4,
							bone->mOffsetMatrix.c1, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.c4,
							bone->mOffsetMatrix.d1, bone->mOffsetMatrix.d2, bone->mOffsetMatrix.d3, bone->mOffsetMatrix.d4);

	for (uint i = 0; i < rBone->numWeights; i++)
		memcpy(rBone->weights + i, &((bone->mWeights + i)->mWeight), sizeof(float));

	for (uint i = 0; i < rBone->numWeights; i++)
		memcpy(rBone->weightsIndex + i, &((bone->mWeights + i)->mVertexId), sizeof(uint));

	rBone->ID = ID;
	rBone->name = bone->mName.C_Str();

	std::string full_path("/Library/Bones/");
	full_path.append(std::to_string(ID));
	rBone->resource_file = full_path;
	rBone->original_file = source_file;
	rBone->LoadOnMemory();
	SaveBoneResource(rBone);

	return rBone;
}

bool M_Animations::SaveBoneResource(R_Bone* bone)
{
	uint size = sizeof(uint) * 2 + bone->original_file.size() + bone->name.size() + sizeof(uint) + sizeof(uint) + sizeof(bone->numWeights	) * sizeof(float) + sizeof(bone->numWeights) * sizeof(uint)
		+ sizeof(float) * 16;

	char* data = new char[size];
	char* cursor = data;

	// Store source file and name: Sizes;
	uint fileSize = bone->original_file.size();
	memcpy(cursor, &fileSize, sizeof(uint));
	cursor += sizeof(uint);

	uint nameSize = bone->name.size();
	memcpy(cursor, &nameSize, sizeof(uint));
	cursor += sizeof(uint);

	// Store source file and name: Strings
	memcpy(cursor, bone->original_file.c_str(), bone->original_file.size());
	cursor += bone->original_file.size();

	memcpy(cursor, bone->name.c_str(), bone->name.size());
	cursor += bone->name.size();

	memcpy(cursor, &bone->meshID, sizeof(uint));
	cursor += sizeof(uint);

	//Num weights
	memcpy(cursor, &bone->numWeights, sizeof(uint));
	cursor += sizeof(uint);

	//Weights
	memcpy(cursor, bone->weights, sizeof(float) * sizeof(bone->numWeights));
	cursor += sizeof(float) * sizeof(bone->numWeights);

	//Weights index
	memcpy(cursor, bone->weightsIndex, sizeof(uint) * sizeof(bone->numWeights));
	cursor += sizeof(uint) * sizeof(bone->numWeights);

	//Offset matrix
	memcpy(cursor, &bone->offset, sizeof(float) * 16);
	cursor += sizeof(float) * 16;

	uint ret = App->fileSystem->Save(bone->resource_file.c_str(), data, size);
	RELEASE_ARRAY(data);

	return ret > 0;
}

R_Bone* M_Animations::LoadBone(uint64 ID)
{
	std::string full_path = "/Library/Bones/";
	full_path.append(std::to_string(ID));

	char* buffer;
	uint size = App->fileSystem->Load(full_path.c_str(), &buffer);

	if (size > 0)
	{
		char* cursor = buffer;
		R_Bone* bone = new R_Bone();

		uint stringSize = 0;
		memcpy(&stringSize, cursor, sizeof(uint));
		cursor += sizeof(uint);

		uint nameSize = 0;
		memcpy(&nameSize, cursor, sizeof(uint));
		cursor += sizeof(uint);

		if (stringSize > 0)
		{
			char* string = new char[stringSize + 1];
			memcpy(string, cursor, sizeof(char) * stringSize);
			cursor += sizeof(char) * stringSize;
			string[stringSize] = '\0';
			bone->original_file = string;
			RELEASE_ARRAY(string);
		}

		if (nameSize > 0)
		{
			char* string = new char[nameSize + 1];
			memcpy(string, cursor, sizeof(char) * nameSize);
			cursor += sizeof(char) * nameSize;
			string[nameSize] = '\0';
			bone->name = string;
			RELEASE_ARRAY(string);
		}

		memcpy(&bone->numWeights, cursor, sizeof(uint));
		cursor += sizeof(uint);

		memcpy(&bone->weights, cursor, sizeof(float) * bone->numWeights);
		cursor += sizeof(float) * bone->numWeights;

		memcpy(&bone->weightsIndex, cursor, sizeof(uint) * bone->numWeights);
		cursor += sizeof(uint) * bone->numWeights;

		float* offset = new float[16];
		memcpy(offset, cursor, sizeof(float) * 16);
		cursor += sizeof(float) * 16;
		bone->offset = float4x4(offset[0], offset[1], offset[2], offset[3],
								offset[4], offset[5], offset[6], offset[7], 
								offset[8], offset[9], offset[10], offset[11], 
								offset[12], offset[13], offset[14], offset[15]);

		RELEASE_ARRAY(offset);

		bone->ID = ID;
		bone->resource_file = full_path;

		RELEASE_ARRAY(buffer);
		return bone;
	}
	else
	{
		return nullptr;
	}
}

uint M_Animations::CalcChannelSize(const Channel& channel) const
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

void M_Animations::SaveChannelData(const Channel& channel, char** cursor)
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

	SaveKeys(channel.positionKeys, cursor);
	SaveKeys(channel.rotationKeys, cursor);
	SaveKeys(channel.scaleKeys, cursor);
}

void M_Animations::SaveKeys(const std::map<double, float3>& map, char** cursor)
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

void M_Animations::SaveKeys(const std::map<double, Quat>& map, char** cursor)
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

void M_Animations::LoadChannelData(Channel& channel, char** cursor)
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

	LoadKeys(channel.positionKeys, cursor, ranges[0]);
	LoadKeys(channel.rotationKeys, cursor, ranges[1]);
	LoadKeys(channel.scaleKeys, cursor, ranges[2]);
}

void M_Animations::LoadKeys(std::map<double, float3>& map, char** cursor, uint size)
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

void M_Animations::LoadKeys(std::map<double, Quat>& map, char** cursor, uint size)
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

bool M_Animations::Init(Config& config)
{
	return true;
}

bool M_Animations::CleanUp()
{
	return true;
}

void M_Animations::CollectGameObjectNames(GameObject* gameObject, std::map<std::string, GameObject*>& map)
{
	map[gameObject->name.c_str()] = gameObject;
	for (uint i = 0; i < gameObject->childs.size(); i++)
		CollectGameObjectNames(gameObject->childs[i], map);
}

void M_Animations::LoadChannel(const aiNodeAnim* node, Channel& channel)
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