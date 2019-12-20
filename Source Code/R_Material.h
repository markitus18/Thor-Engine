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
	uint64 shaderID = 1567607513; // TODO: change into a default shader
	uint64 textureID = 0;
	Color color;
};

#endif //__R_MATERIAL_H__