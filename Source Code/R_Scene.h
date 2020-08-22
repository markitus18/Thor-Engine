#ifndef __R_SCENE_H__
#define __R_SCENE_H__

#include "Resource.h"

class GameObject;
class C_Camera;

class R_Scene : public Resource
{
public:
	R_Scene();
	~R_Scene();

public:
	GameObject* root = nullptr;

	C_Camera* editorCamera = nullptr;
	C_Camera* mainCamera = nullptr;

	uint64 thumbnailID = 0;
};

#endif //!__R_SCENE_H__
