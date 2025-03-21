#ifndef __R_ANIMATOR_H__
#define __R_ANIMATOR_H__

#include "Resource.h"
#include "ResourceHandle.h"
#include "R_Animation.h"

#include "Globals.h"

#include <vector>

class R_AnimatorController : public Resource
{
public:
	R_AnimatorController();
	~R_AnimatorController();

	void AddAnimation();
	void AddAnimation(uint64 animationID);

public:
	std::vector<ResourceHandle<R_Animation>> animations;
};

#endif
