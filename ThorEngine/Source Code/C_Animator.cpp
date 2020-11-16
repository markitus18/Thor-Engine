#include "C_Animator.h"

#include "Engine.h"
#include "M_Renderer3D.h"

#include "M_Resources.h"

#include "GameObject.h"

#include "R_AnimatorController.h"
#include "R_Animation.h"

#include "C_Transform.h"
#include "C_Mesh.h"

#include "Time.h"

typedef std::map<double, float3>::const_iterator key_f3;
typedef std::map<double, Quat>::const_iterator key_Quat;

C_Animator::C_Animator(GameObject* gameObject) : Component(Type::Animator, gameObject, true)
{

}

C_Animator::~C_Animator()
{

}

void C_Animator::DrawLinkedBones() const
{
	/*
	for (uint i = 0; i < links.size(); i++)
	{
		C_Transform* transform;// = (C_Transform*)links[i].gameObject->GetComponent<C_Transform>();
		float3 pos = transform->GetGlobalPosition();
		AABB* box = new AABB();
		box->SetFromCenterAndSize(pos, float3(1, 1, 1));
		Engine->renderer3D->AddAABB(*box, Pink);
	}
	*/
}

void C_Animator::Start()
{
	//TODO: hard-coding root bone for fast code iteration
	rootBone = gameObject->childs[1]->childs[0]; //Should be childs[0] childs[0] in skeleton mesh
	gameObject->childs[0]->GetComponent<C_Mesh>()->rootBone = rootBone;

	if (rootBone == nullptr) return;

	boneMapping.clear();

	std::vector<GameObject*> bones;
	rootBone->CollectChilds(bones);

	for (uint i = 0; i < bones.size(); ++i)
	{
		boneMapping[bones[i]->name] = bones[i];
	}
}

void C_Animator::Update(float dt)
{
	dt = Time::deltaTime;
	//"if" not necessary but we avoid all calculations
	if (dt > 0.0f)
	{
		if (playing == true)
		{
			if (started == false)
				Start();

			R_Animation* prevAnimation = GetAnimation(previous_animation);
			R_Animation* currentAnimation = GetAnimation(current_animation);

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
				else if (prevAnimation && prevAnimTime >= prevAnimation->duration)
				{
					if (prevAnimation->loopable == true)
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

			if (currentAnimation && time > currentAnimation->duration)
			{
				if (currentAnimation->loopable == true)
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

			UpdateChannelsTransform(currentAnimation, blendRatio > 0.0f ? prevAnimation : nullptr, blendRatio);
			UpdateMeshAnimation(gameObject->childs[0]);
		}
	}
}

void C_Animator::Draw(RenderingSettings::RenderingFlags flags)
{
	if (flags & RenderingSettings::RenderFlag_Bones)
		DrawLinkedBones();
}

void C_Animator::Serialize(Config& config)
{
	Component::Serialize(config);
	if (config.isSaving)
	{
		config.SetNumber("Animator Resource", rAnimatorHandle.GetID());
		config.SetBool("Playing", playing);
		config.SetNumber("Current Animation", current_animation);

	}
	else
	{
		uint64 resourceID = config.GetNumber("Animator Resource");
		SetResource(resourceID);

		playing = config.GetBool("Playing");
		SetAnimation(config.GetNumber("Current Animation"));
	}
}

void C_Animator::SetAnimation(const char* name, float blendTime)
{
	R_AnimatorController* rAnimator = rAnimatorHandle.Get();
	for (uint i = 0; i < rAnimator->animations.size(); ++i)
	{
		//TODO: Animations should be loaded with animator controller
		/*
		R_Animation* animation = (R_Animation*)Engine->moduleResources->GetResource(rAnimator->animations[i]);
		if (animation && animation->name == name)
		{
			SetAnimation(i, blendTime);
			break;
		}
		*/
	}
}

void C_Animator::SetAnimation(uint index, float blendTime)
{
	R_Animation* newAnimation = GetAnimation(index);
	R_Animation* currentAnimation = GetAnimation(current_animation);

	if (newAnimation)
	{
		if (currentAnimation && index != current_animation)
		{
			if (blendTime > 0 && playing == true)
			{
				previous_animation = current_animation;
				prevAnimTime = time;
				blendTimeDuration = blendTime;
				this->blendTime = 0.0f;
			}
		}
		current_animation = index;
		time = 0.0f;
		playing = true;
	}
}

R_Animation* C_Animator::GetAnimation(uint index)
{
	R_AnimatorController* rAnimator = rAnimatorHandle.Get();
	if (index < rAnimator->animations.size())
	{
		//TODO: Animations should be all loaded with the animator controller
		//return (R_Animation*)Engine->moduleResources->GetResource(rAnimator->animations[index]);
		return nullptr;
	}
}

void C_Animator::SetResource(Resource* resource)
{
	rAnimatorHandle.Set((R_AnimatorController*)resource);
}

void C_Animator::SetResource(unsigned long long ID)
{
	rAnimatorHandle.Set(ID);
}

uint64 C_Animator::GetResourceID() const
{
	return rAnimatorHandle.GetID();
}

void C_Animator::UpdateChannelsTransform(const R_Animation* settings, const R_Animation* blend, float blendRatio)
{
	uint currentFrame = settings->ticksPerSecond * time;

	uint prevBlendFrame = 0;
	if (blend != nullptr)
	{
		prevBlendFrame = blend->ticksPerSecond * prevAnimTime;
	}

	std::map<std::string, GameObject*>::iterator boneIt;
	for (boneIt = boneMapping.begin(); boneIt != boneMapping.end(); ++boneIt)
	{
		C_Transform* transform = boneIt->second->GetComponent<C_Transform>();

		//TODO: Searching here for strings. Bone mapping should handle this at start
		if (settings->channels.find(boneIt->first.c_str()) == settings->channels.end()) continue;

		const Channel& channel = settings->channels.find(boneIt->first.c_str())->second;

		float3 position = GetChannelPosition(channel, currentFrame, transform->GetLocalPosition());
		Quat rotation = GetChannelRotation(channel, currentFrame, transform->GetLocalRotation());
		float3 scale = GetChannelScale(channel, currentFrame, transform->GetLocalScale());

		if (blend != nullptr)
		{
			const Channel& blendChannel = blend->channels.find(boneIt->first.c_str())->second;

			position = float3::Lerp(GetChannelPosition(blendChannel, prevBlendFrame, transform->GetLocalPosition()), position, blendRatio);
			rotation = Quat::Slerp(GetChannelRotation(blendChannel, prevBlendFrame, transform->GetLocalRotation()), rotation, blendRatio);
			scale = float3::Lerp(GetChannelScale(blendChannel, prevBlendFrame, transform->GetLocalScale()), scale, blendRatio);
		}

		transform->SetLocalPosition(position);
		transform->SetLocalRotation(rotation);
		transform->SetLocalScale(scale);
	}
}

float3 C_Animator::GetChannelPosition(const Channel& channel, float currentKey, float3 default) const
{
	float3 position = default;
	
	if (channel.HasPosKey())
	{
		key_f3 previous = channel.GetPrevPosKey(currentKey);
		key_f3 next = channel.GetNextPosKey(currentKey);

		if (next == channel.positionKeys.end())
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

Quat C_Animator::GetChannelRotation(const Channel& channel, float currentKey, Quat default) const
{
	Quat rotation = default;

	if (channel.HasRotKey())
	{
		key_Quat previous = channel.GetPrevRotKey(currentKey);
		key_Quat next = channel.GetNextRotKey(currentKey);

		if (next == channel.rotationKeys.end())
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

float3 C_Animator::GetChannelScale(const Channel& channel, float currentKey, float3 default) const
{
	float3 scale = default;

	if (channel.HasScaleKey())
	{
		key_f3 previous = channel.GetPrevScaleKey(currentKey);
		key_f3 next = channel.GetPrevScaleKey(currentKey);

		if (next == channel.scaleKeys.end())
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

void C_Animator::UpdateMeshAnimation(GameObject* gameObject)
{
	C_Mesh* mesh = gameObject->GetComponent<C_Mesh>();
	if (mesh != nullptr)
	{
		mesh->StartBoneDeformation();
		mesh->DeformAnimMesh();
		//Engine->renderer3D->LoadBuffers(mesh->animMesh);
	}

	for (uint i = 0; i < gameObject->childs.size(); i++)
		UpdateMeshAnimation(gameObject->childs[i]);
}