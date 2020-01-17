#ifndef __C_ANIMATOR_H__
#define __C_ANIMATOR_H__

#include "Component.h"
#include <vector>
#include <map>

class GameObject;
class Channel;
class C_Mesh;
class R_Animation;

class C_Animator : public Component
{
public:
	C_Animator(GameObject* gameObject);
	~C_Animator();

	void DrawLinkedBones() const;

	void Start();
	void Update(float dt);

	//Animation configuration management
	void AddAnimation();
	void AddAnimation(uint64 animationID);

	void SetAnimation(uint index, float blendTime = 0.0f);
	void SetAnimation(const char* name, float blendTime = 0.0f);

	static inline Type GetType() { return Type::Animator; };

private:

	void UpdateChannelsTransform(const R_Animation* settings, const R_Animation* blend, float blendRatio);
	float3 GetChannelPosition(const Channel& channel, float currentKey, float3 default) const;
	Quat GetChannelRotation(const Channel& channel, float currentKey, Quat default) const;
	float3 GetChannelScale(const Channel& channel, float currentKey, float3 default) const;

	void UpdateMeshAnimation(GameObject* gameObject);


public:
	GameObject* rootBone = nullptr;
	std::vector<uint64> animations;
	std::map<std::string, GameObject*> boneMapping;

	//Used for blending
	uint previous_animation = 0;
	uint current_animation = 0;
	bool playing = false;

private:
	bool started = false;

	float prevAnimTime = 0.0f;
	float time = 0.0f;
	float blendTime = 0.0f;
	float blendTimeDuration = 0.0f;

	bool channelsLinked = false;
	bool bonesLinked = false;
};

#endif // __C_ANIMATOR_H__
