#ifndef __C_ANIMATION_H__
#define __C_ANIMATION_H__

#include "Component.h"
#include <vector>

class GameObject;
class Channel;

class C_Animation : public Component
{
	//Careful, this could be dangerous
	struct Link
	{
		Link(GameObject* gameObject, Channel* channel) : gameObject(gameObject), channel(channel) {};
		GameObject* gameObject;
		Channel* channel;
	};

public:
	C_Animation(GameObject* gameObject);
	~C_Animation();

	void LinkChannels();
	void DrawLinkedBones() const;

	static Component::Type GetType();

private:
	std::vector<Link> links;

};

#endif // __C_ANIMATION_H__
