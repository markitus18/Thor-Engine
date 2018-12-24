#include "C_Animation.h"
#include "R_Animation.h"
#include "R_Bone.h"

#include "Application.h"
#include "M_Renderer3D.h"

#include "GameObject.h"

float AnimationSettings::GetDuration()
{
	return ((float)end_frame - (float)start_frame) / ticksPerSecond;
}

C_Animation::C_Animation(GameObject* gameObject) : Component(Type::Animation, gameObject, true)
{

}

C_Animation::~C_Animation()
{

}

void C_Animation::LinkChannels()
{
	std::vector<GameObject*> gameObjects;
	gameObject->CollectChilds(gameObjects);

	R_Animation* rAnimation = (R_Animation*)GetResource();
	for (uint i = 0; i < rAnimation->numChannels; i++)
	{
		for (uint g = 0; g < gameObjects.size(); g++)
		{
			if (gameObjects[g]->name == rAnimation->channels[i].name.c_str() && gameObjects[g]->HasComponent(Component::Type::Bone))
			{
				links.push_back(Link(gameObjects[g], &rAnimation->channels[i]));
				break;
			}
		}
	}
	channelsLinked = true;
	bonesLinked = false;
}

void C_Animation::LinkBones()
{
	std::map<uint64, C_Mesh*> meshes;
	std::vector<C_Bone*> bones;
	CollectMeshesBones(gameObject, meshes, bones);

	for (uint i = 0; i < bones.size(); i++)
	{
		uint64 meshID = ((R_Bone*)bones[i]->GetResource())->meshID;
		std::map<uint64, C_Mesh*>::iterator it = meshes.find(meshID);
		if (it != meshes.end())
		{
			it->second->AddBone(bones[i]);
		}
	}
	bonesLinked = true;
}

void C_Animation::DrawLinkedBones() const
{
	for (uint i = 0; i < links.size(); i++)
	{
		C_Transform* transform = (C_Transform*)links[i].gameObject->GetComponent<C_Transform>();
		float3 pos = transform->GetGlobalPosition();
		AABB* box = new AABB();
		box->SetFromCenterAndSize(pos, float3(1, 1, 1));
		App->renderer3D->AddAABB(*box, Pink);
	}
}

void C_Animation::Start()
{
	if (channelsLinked == false)
	{
		LinkChannels();
	}
	if (bonesLinked == false)
	{
		LinkBones();
	}
	started = true;

	animations[current_animation].current = true;

}

void C_Animation::Update(float dt)
{
	dt = Time::deltaTime;
	//"if" not necessary but we avoid all calculations
	if (dt > 0.0f)
	{
		if (playing == true)
		{
			if (started == false)
				Start();
			R_Animation* resource = (R_Animation*)GetResource();


			//Updating animation blend
			float blendRatio = 0.0f;
			if (blendTimeDuration > 0.0f)
			{
				prevAnimTime += dt;
				blendTime += dt;

				if (blendTime >= blendTimeDuration)
				{
					blendTimeDuration = 0.0f;
				}

				else if (prevAnimTime >= animations[previous_animation].GetDuration())
				{
					if (animations[previous_animation].loopable == true)
					{
						prevAnimTime = 0.0f;
						// + (currentFrame - endFrame);
					}
				}

				if (blendTimeDuration > 0.0f)
					blendRatio = blendTime / blendTimeDuration;
			}
			//Endof Updating animation blend
		
			time += dt;

			if (time > animations[current_animation].GetDuration())
			{
				if (animations[current_animation].loopable == true)
				{
					time = 0.0f;
				}
				else
				{
					playing = false;
					//TODO: is it really necessary? Not returning could end in last anim frame
					return;
				}
			}

			UpdateChannelsTransform(&animations[current_animation], blendRatio > 0.0f ? &animations[previous_animation] : nullptr, blendRatio);
			UpdateMeshAnimation(gameObject);
		}
	}
}

void C_Animation::AddAnimation()
{
	//Just some simple and stupid way to avoid name duplication
	std::string new_name = "Default";
	uint defCount = 0;
	for (uint i = 0; i < animations.size(); i++)
	{
		if (animations[i].name.find("Default") != std::string::npos)
			defCount++;
	}
	if (defCount > 0)
		new_name.append(std::to_string(defCount));

	R_Animation* rAnim = (R_Animation*)GetResource();
	AddAnimation(new_name.c_str(), 0, rAnim->duration, 24);
}

void C_Animation::AddAnimation(const char* name, uint init, uint end, float ticksPerSec)
{
	AnimationSettings animation;
	animation.name = name;
	animation.start_frame = init;
	animation.end_frame = end;
	animation.ticksPerSecond = ticksPerSec;

	animations.push_back(animation);
}

void C_Animation::SetAnimation(uint index, float blendTime)
{
	if (index < animations.size())
	{
		if (current_animation < animations.size() && index != current_animation)
		{
			animations[current_animation].current = false;

			if (blendTime > 0 && playing == true)
			{
				previous_animation = current_animation;
				prevAnimTime = time;
				blendTimeDuration = blendTime;
				this->blendTime = 0.0f;
			}
		}
		current_animation = index;
		animations[current_animation].current = true;
		time = 0.0f;
		playing = true;
	}
}

void C_Animation::SetAnimation(const char* name, float blendTime)
{
	if (animations[current_animation].name != name)
	{
		for (uint i = 0; i < animations.size(); i++)
		{
			if (animations[i].name == name)
			{
				SetAnimation(i, blendTime);
				return;
			}
		}
	}
}

Component::Type C_Animation::GetType()
{
	return Component::Type::Animation;
}

void C_Animation::UpdateChannelsTransform(const AnimationSettings* settings, const AnimationSettings* blend, float blendRatio)
{
	R_Animation* resource = (R_Animation*)GetResource();
	uint currentFrame = settings->start_frame + (settings->ticksPerSecond > 0.0f ? resource->ticksPerSecond : settings->ticksPerSecond) * time;

	uint prevBlendFrame = 0;
	if (blend != nullptr)
	{
		prevBlendFrame = blend->start_frame + (blend->ticksPerSecond > 0.0f ? resource->ticksPerSecond : blend->ticksPerSecond) * prevAnimTime;
	}

	for (uint i = 0; i < links.size(); i++)
	{
		C_Transform* transform = (C_Transform*)links[i].gameObject->GetComponent<C_Transform>();

		float3 position = GetChannelPosition(links[i], currentFrame, transform->GetPosition(), *settings);
		Quat rotation = GetChannelRotation(links[i], currentFrame, transform->GetQuatRotation(), *settings);
		float3 scale = GetChannelScale(links[i], currentFrame, transform->GetScale(), *settings);

		if (blend != nullptr)
		{
			position = float3::Lerp(GetChannelPosition(links[i], prevBlendFrame, transform->GetPosition(), *blend), position, blendRatio);
			rotation = Quat::Slerp(GetChannelRotation(links[i], prevBlendFrame, transform->GetQuatRotation(), *blend), rotation, blendRatio);
			scale = float3::Lerp(GetChannelScale(links[i], prevBlendFrame, transform->GetScale(), *blend), scale, blendRatio);
		}

		transform->SetPosition(position);
		transform->SetQuatRotation(rotation);
		transform->SetScale(scale);
	}
}

float3 C_Animation::GetChannelPosition(Link& link, float currentKey, float3 default, const AnimationSettings& settings)
{
	float3 position = default;

	if (link.channel->HasPosKey())
	{
		std::map<double, float3>::iterator previous = link.channel->GetPrevPosKey(currentKey, settings.start_frame, settings.end_frame);
		std::map<double, float3>::iterator next = link.channel->GetNextPosKey(currentKey, settings.start_frame, settings.end_frame);

		if (next == link.channel->positionKeys.end())
			next = previous;

		//If both keys are the same, no need to blend
		if (previous == next)
			position = previous->second;
		else //blend between both keys
		{
			//0 to 1
			float ratio = (currentKey - previous->first) / (next->first - previous->first);
			position = previous->second.Lerp(next->second, ratio);
		}
	}

	return position;
}

Quat C_Animation::GetChannelRotation(Link& link, float currentKey, Quat default, const AnimationSettings& settings)
{
	Quat rotation = default;

	if (link.channel->HasRotKey())
	{
		std::map<double, Quat>::iterator previous = link.channel->GetPrevRotKey(currentKey, settings.start_frame, settings.end_frame);
		std::map<double, Quat>::iterator next = link.channel->GetNextRotKey(currentKey, settings.start_frame, settings.end_frame);

		if (next == link.channel->rotationKeys.end())
			next = previous;

		//If both keys are the same, no need to blend
		if (previous == next)
			rotation = previous->second;
		else //blend between both keys
		{
			//0 to 1
			float ratio = (currentKey - previous->first) / (next->first - previous->first);
			rotation = previous->second.Slerp(next->second, ratio);
		}
	}
	return rotation;
}

float3 C_Animation::GetChannelScale(Link& link, float currentKey, float3 default, const AnimationSettings& settings)
{
	float3 scale = default;

	if (link.channel->HasScaleKey())
	{
		std::map<double, float3>::iterator previous = link.channel->GetPrevScaleKey(currentKey, settings.start_frame, settings.end_frame);
		std::map<double, float3>::iterator next = link.channel->GetPrevScaleKey(currentKey, settings.start_frame, settings.end_frame);

		if (next == link.channel->scaleKeys.end())
			next = previous;

		//If both keys are the same, no need to blend
		if (previous == next)
			scale = previous->second;
		else //blend between both keys
		{
			//0 to 1
			float ratio = (currentKey - previous->first) / (next->first - previous->first);
			scale = previous->second.Lerp(next->second, ratio);
		}
	}
	return scale;
}

void C_Animation::CollectMeshesBones(GameObject* gameObject, std::map<uint64, C_Mesh*>& meshes, std::vector<C_Bone*>& bones)
{
	C_Mesh* mesh = gameObject->GetComponent<C_Mesh>();
	if (mesh != nullptr)
	{
		meshes[mesh->GetResource()->GetID()] = mesh;
	}
	C_Bone* bone = gameObject->GetComponent<C_Bone>();
	if (bone != nullptr)
	{
		bones.push_back(bone);
	}

	for (uint i = 0; i < gameObject->childs.size(); i++)
	{
		CollectMeshesBones(gameObject->childs[i], meshes, bones);
	}
}

void C_Animation::UpdateMeshAnimation(GameObject* gameObject)
{
	C_Mesh* mesh = gameObject->GetComponent<C_Mesh>();
	if (mesh != nullptr)
	{
		mesh->StartBoneDeformation();
		mesh->DeformAnimMesh();
		App->renderer3D->LoadBuffers(mesh->animMesh);
	}

	for (uint i = 0; i < gameObject->childs.size(); i++)
		UpdateMeshAnimation(gameObject->childs[i]);
}

void C_Animation::ClearDefaultAnimation()
{
	if (animations.size() == 1 && animations[0].name == "Default")
	{
		animations.erase(animations.begin());
	}
}