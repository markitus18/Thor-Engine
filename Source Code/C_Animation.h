#ifndef __C_ANIMATION_H__
#define __C_ANIMATION_H__

#include "Component.h"
#include <vector>
#include <map>

class GameObject;
class Channel;
class C_Mesh;
class C_Bone;

struct AnimationSettings
{
	std::string name;
	uint start_fame;
	uint end_frame;
	float speed;
	bool loopable;
};

class C_Animation : public Component
{
	//Careful, this could be dangerous, duplicating pointers
	struct Link
	{
		Link(GameObject* gameObject, Channel* channel) : gameObject(gameObject), channel(channel) {};
		GameObject* gameObject;
		Channel* channel;

		std::map<double, float3>::iterator prevPosKey;
		std::map<double, Quat>::iterator prevRotKey;
		std::map<double, float3>::iterator prevScaleKey;
	};

public:
	C_Animation(GameObject* gameObject);
	~C_Animation();

	void LinkChannels();
	void LinkBones();
	void DrawLinkedBones() const;

	void Start();
	void Update(float dt);

	//Animation configuration management
	void AddAnimation();
	void AddAnimation(const char* name, uint init, uint end, float ticksPerSec);

	static Component::Type GetType();

private:

	void UpdateChannelsTransform(float dt, const AnimationSettings& settings);
	float3 GetChannelPosition(Link& link, float currentKey, float3 default, const AnimationSettings& settings);
	Quat GetChannelRotation(Link& link, float currentKey, Quat default, const AnimationSettings& settings);
	float3 GetChannelScale(Link& link, float currentKey, float3 default, const AnimationSettings& settings);

	void CollectMeshesBones(GameObject* gameObject, std::map<uint64, C_Mesh*>& meshes, std::vector<C_Bone*>& bones);
	void UpdateMeshAnimation(GameObject* gameObject);

	void ClearDefaultAnimation();

public:
	std::vector<AnimationSettings> animations;
	uint current_animation = 0;
	bool playing = false;

private:
	bool started = false;

	float currentFrame = 0.0f;
	std::vector<Link> links;

	bool channelsLinked = false;
	bool bonesLinked = false;

};

#endif // __C_ANIMATION_H__
