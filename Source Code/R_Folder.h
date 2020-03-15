#ifndef __R_BONE_H__
#define __R_BONE_H__

#include "Resource.h"
#include <vector>

class R_Folder : public Resource
{
public:
	R_Folder();
	~R_Folder();

	inline void AddResource(uint64 resourceID) { containingResources.push_back(resourceID); };

public: //TODO: resources should be private, changed for fast iteration
	std::vector<uint64> containingResources;
private:
};

#endif __R_BONE_H__