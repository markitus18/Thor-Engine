#ifndef __M_ANIMATIONS_H__
#define __M_ANIMATIONS_H__

#include "Module.h"
#include <string>
#include <map>

class R_Animation;
class GameObject;
struct aiScene;
struct aiAnimation;

class M_Animations : public Module
{
public:
	M_Animations(bool start_enabled = true);
	~M_Animations();

	void ImportSceneAnimations(const aiScene* scene, GameObject* root, const char* source_file);
	R_Animation* ImportAnimation(const aiAnimation* animation, uint64 ID, const char* source_file);

	bool Init(Config& config);
	bool CleanUp();

private:
	void CollectGameObjectNames(GameObject* gameObject, std::map<std::string, GameObject*>& map);
};

#endif