#ifndef __C_ANIMATION_H__
#define __C_ANIMATION_H__

#include "Component.h"
#include <vector>
#include <map>

class GameObject;
class Channel;
class C_Mesh;
class C_Bone;

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
	static Component::Type GetType();

private:

	void UpdateChannelsTransform(float dt);
	float3 GetChannelPosition(Link& link, float currentKey, float3 default);
	Quat GetChannelRotation(Link& link, float currentKey, Quat default);
	float3 GetChannelScale(Link& link, float currentKey, float3 default);

	void CollectMeshesBones(GameObject* gameObject, std::map<uint64, C_Mesh*>& meshes, std::vector<C_Bone*>& bones);
	void UpdateMeshAnimation(GameObject* gameObject);
	
private:
	bool started = false;
	bool playing = true;
	float currentFrame = 0.0f;
	std::vector<Link> links;

	bool channelsLinked = false;
	bool bonesLinked = false;

};

#endif // __C_ANIMATION_H__
