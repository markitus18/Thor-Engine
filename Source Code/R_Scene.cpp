#include "R_Scene.h"

#include "GameObject.h"

R_Scene::R_Scene() : Resource(Resource::SCENE)
{
	root = new GameObject(nullptr, "root");
	root->hierarchyOpen = true;
	root->uid = 0;
}

R_Scene::~R_Scene()
{

}