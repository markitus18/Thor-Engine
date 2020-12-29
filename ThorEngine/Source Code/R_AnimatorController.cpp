#include "R_AnimatorController.h"

R_AnimatorController::R_AnimatorController() : Resource(ResourceType::ANIMATOR_CONTROLLER)
{
	
}

R_AnimatorController::~R_AnimatorController()
{

}

void R_AnimatorController::AddAnimation()
{
	animations.push_back(ResourceHandle<R_Animation>(nullptr));
}

void R_AnimatorController::AddAnimation(uint64 animationID)
{
	animations.push_back(animationID);
}