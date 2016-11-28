#ifndef __R_MATERIAL_H__
#define __R_MATERIAL_H__

#include "Resource.h"
#include "Color.h"
#include "Globals.h"

#include <string>

class R_Texture;

class R_Material : public Resource
{
public:
	R_Material();
	~R_Material();

public:
	R_Texture* texture;
	uint64 textureID;
	Color color;
};

#endif //__R_MATERIAL_H__