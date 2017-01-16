#ifndef __R_TEXTURE_H__
#define __R_TEXTURE_H__

#include "Resource.h"

class R_Texture : public Resource
{
public:
	R_Texture();
	~R_Texture();

	void LoadOnMemory();
	void FreeMemory();

public:
	uint buffer = 0;
};
#endif //__R_TEXTURE_H__
