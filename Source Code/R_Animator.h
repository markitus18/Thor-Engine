#ifndef __R_ANIMATOR_H__
#define __R_ANIMATOR_H__

#include "Resource.h"
#include "Globals.h"

#include <vector>

class R_Animator : public Resource
{
public:
	R_Animator();
	~R_Animator();

public:
	std::vector<uint64> animations;
};

#endif
