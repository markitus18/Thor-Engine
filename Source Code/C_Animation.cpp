#include "C_Animation.h"
#include "R_Animation.h"
#include "R_Bone.h"

#include "Application.h"
#include "M_Renderer3D.h"

#include "GameObject.h"


C_Animation::C_Animation(GameObject* gameObject) : Component(Type::Animation, gameObject, true)
{
	AddAnimation();
	//TODO: ask Ricard about why a cast is needed
	SetAnimation((uint)0);
	AddAnimation("Default02", 0, 0, 24);
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
	for (uint i = 0; i < links.size(); i++)
	{
		links[i].prevPosKey = links[i].channel->positionKeys.begin();
		links[i].prevRotKey = links[i].channel->rotationKeys.begin();
		links[i].prevScaleKey = links[i].channel->scaleKeys.begin();
	}
	started = true;
	animations[current_animation].current = true;

}

void C_Animation::Update(float dt)
{
	if (playing == true)
	{
		if (started == false)
			Start();
		R_Animation* resource = (R_Animation*)GetResource();

		currentFrame += (animations[current_animation].speed == 0 ? resource->ticksPerSecond : animations[current_animation].speed) * dt;

		uint endFrame = (animations[current_animation].end_frame == 0) ? resource->duration : animations[current_animation].end_frame;
		if (currentFrame >= endFrame)
		{
			if (animations[current_animation].loopable == true)
			{
				currentFrame = animations[current_animation].start_fame;// + (currentFrame - endFrame);
			}
			else
			{
				playing = false;
				return;
			}
		}

		UpdateChannelsTransform(animations[current_animation]);
		UpdateMeshAnimation(gameObject);
	}

	

}

void C_Animation::AddAnimation()
{
	std::string new_name = "Default";
//	for (uint i = 0; i < animations.size(); i++)
//	{
//		if (animations[i].find("Default"))
//	}
	AddAnimation(new_name.c_str(), 0, 0, 24);
}

void C_Animation::AddAnimation(const char* name, uint init, uint end, float ticksPerSec)
{
	AnimationSettings animation;
	animation.name = name;
	animation.start_fame = init;
	animation.end_frame = end;
	animation.speed = ticksPerSec;

	animations.push_back(animation);
}

//TODO: restart animation even when it is already playing?
void C_Animation::SetAnimation(uint index, float blendTime)
{
	if (index != current_animation && index < animations.size())
	{
		if (current_animation < animations.size())
		{
			animations[current_animation].current = false;

			if (blendTime > 0)
			{
				previous_animation = current_animation;
				prevBlendFrame = currentFrame;
				currentBlend = 0;
				blendDuration = animations[current_animation].speed * blendTime;
			}
		}
		current_animation = index;
		animations[current_animation].current = true;
		currentFrame = animations[current_animation].start_fame;
	}
}

//TODO: restart animation even when it is already playing?
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

void C_Animation::UpdateChannelsTransform(const AnimationSettings& settings)
{
	for (uint i = 0; i < links.size(); i++)
	{
		C_Transform* transform = (C_Transform*)links[i].gameObject->GetComponent<C_Transform>();

		float3 position = GetChannelPosition(links[i], currentFrame, transform->GetPosition(), settings);
		Quat rotation = GetChannelRotation(links[i], currentFrame, transform->GetQuatRotation(), settings);
		float3 scale = GetChannelScale(links[i], currentFrame, transform->GetScale(), settings);


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
		std::map<double, float3>::iterator previous = link.channel->GetPrevPosKey(link.prevPosKey, currentKey, settings.start_fame, settings.end_frame);
		std::map<double, float3>::iterator next = link.channel->GetNextPosKey(link.prevPosKey, currentKey, settings.start_fame, settings.end_frame);
		link.prevPosKey = previous;

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
		std::map<double, Quat>::iterator previous = link.channel->GetPrevRotKey(link.prevRotKey, currentKey, settings.start_fame, settings.end_frame);
		std::map<double, Quat>::iterator next = link.channel->GetNextRotKey(link.prevRotKey, currentKey, settings.start_fame, settings.end_frame);
		link.prevRotKey = previous;

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
		std::map<double, float3>::iterator previous = link.channel->GetPrevScaleKey(link.prevScaleKey, currentKey, settings.start_fame, settings.end_frame);
		std::map<double, float3>::iterator next = link.channel->GetPrevScaleKey(link.prevScaleKey, currentKey, settings.start_fame, settings.end_frame);
		link.prevScaleKey = previous;

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