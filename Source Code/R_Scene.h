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

	void Update(float dt);

public:
	GameObject* root = nullptr;

	C_Camera* editorPerspCamera = nullptr;
	C_Camera* editorOrthoCamera = nullptr;
	C_Camera* mainCamera = nullptr;
};

#endif //!__R_SCENE_H__
