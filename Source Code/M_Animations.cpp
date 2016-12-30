#include "M_Animations.h"
#include "Application.h"
#include "M_Resources.h"
#include "GameObject.h"

#include "R_Animation.h"

#include "Assimp/include/scene.h"

M_Animations::M_Animations(bool start_enabled) : Module("AnimationImporter", start_enabled)
{

}
M_Animations::~M_Animations()
{

}

void M_Animations::ImportSceneAnimations(const aiScene* scene, GameObject* root, const char* source_file)
{
	if (scene->HasAnimations() == true)
	{
		std::map<std::string, GameObject*> map;
		CollectGameObjectNames(root, map);

		for (uint i = 0; i < scene->mNumAnimations; i++)
		{
			uint ID = App->moduleResources->ImportRAnimation(scene->mAnimations[i], source_file, scene->mAnimations[i]->mName.C_Str());
		}
	}
}

R_Animation* M_Animations::ImportAnimation(const aiAnimation* anim, uint64 ID, const char* source_file)
{
	R_Animation* animation = new R_Animation();
	//animation->numPositionKeys = anim->
	return animation;
}

bool M_Animations::Init(Config& config)
{
	return true;
}

bool M_Animations::CleanUp()
{
	return true;
}

void M_Animations::CollectGameObjectNames(GameObject* gameObject, std::map<std::string, GameObject*>& map)
{
	map[gameObject->name.c_str()] = gameObject;
	for (uint i = 0; i < gameObject->childs.size(); i++)
		CollectGameObjectNames(gameObject->childs[i], map);
}