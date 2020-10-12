#ifndef __R_SCENE_H__
#define __R_SCENE_H__

#include "Resource.h"

//R_Map is only used as a transitional resource to load a map file and add it to a scene
class R_Map : public Resource
{
public:
	R_Map();
	~R_Map();

public:
	Config config;
};

#endif //!__R_SCENE_H__
