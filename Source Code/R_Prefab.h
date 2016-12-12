#ifndef __R_PREFAB_H__
#define __R_PREFAB_H__

#include "Resource.h"

class R_Prefab : public Resource
{
public:
	R_Prefab();
	~R_Prefab();

	uint64 miniTextureID = 0;
};

#endif __R_PREFAB_H__