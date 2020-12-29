#ifndef __R_ANIMATOR_H__
#define __R_ANIMATOR_H__

#include "Resource.h"
#include "Globals.h"

#include "ResourceHandle.h"
#include "R_Animation.h"

#include <vector>

class R_AnimatorController : public Resource
{
public:
	R_AnimatorController();
	~R_AnimatorController();

	//Animation configuration management
	void AddAnimation();
	void AddAnimation(uint64 animationID);

public:
	std::vector<ResourceHandle<R_Animation>> animations;
	//std::vector<uint64> animations;
};

#endif
