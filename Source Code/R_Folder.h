#ifndef __R_BONE_H__
#define __R_BONE_H__

#include "Resource.h"
#include <vector>

class R_Folder : public Resource
{
public:
	R_Folder();
	~R_Folder();

private:
	std::vector<uint64> containingResources;
};

#endif __R_BONE_H__