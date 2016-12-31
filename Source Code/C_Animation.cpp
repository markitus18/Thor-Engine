#include "C_Animation.h"
#include "R_Animation.h"

#include "Application.h"
#include "M_Renderer3D.h"

#include "GameObject.h"


C_Animation::C_Animation(GameObject* gameObject) : Component(Type::Animation, gameObject, true)
{

}

C_Animation::~C_Animation()
{

}

void C_Animation::LinkChannels()
{
	std::vector<GameObject*> gameObjects;
	gameObject->CollectChilds(gameObjects);

	R_Animation* rAnimation = (R_Animation*)GetResource();
	for (uint i = 0; i < rAnimation->numChannels; i++)
	{
		for (uint g = 0; g < gameObjects.size(); g++)
		{
			if (gameObjects[g]->name == rAnimation->channels[i].name.c_str())
			{
				links.push_back(Link(gameObjects[g], &rAnimation->channels[i]));
				break;
			}
		}
	}
}

void C_Animation::DrawLinkedBones() const
{
	for (uint i = 0; i < links.size(); i++)
	{
		C_Transform* transform = (C_Transform*)links[i].gameObject->GetComponent<C_Transform>();
		float3 pos = transform->GetGlobalPosition();
		AABB* box = new AABB();
		box->SetFromCenterAndSize(pos, float3(1, 1, 1));
		App->renderer3D->AddAABB(*box, Pink);
	}
}

Component::Type C_Animation::GetType()
{
	return Component::Type::Animation;
}