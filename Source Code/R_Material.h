#ifndef __R_MATERIAL_H__
#define __R_MATERIAL_H__

#include "Resource.h"
#include "Color.h"
#include <string>

class R_Texture;

class R_Material : public Resource
{
public:
	R_Material();
	~R_Material();

public:
	std::string name;
	R_Texture* texture;
	unsigned long long textureID;
	Color color;
};

#endif //__R_MATERIAL_H__