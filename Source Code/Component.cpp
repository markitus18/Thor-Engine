#include "Component.h"
#include "Resource.h"
#include "Application.h"
#include "M_Resources.h"

Component::Component(Type type, const GameObject* gameObject) : type(type)
{
	this->gameObject = gameObject;
}

Component::~Component()
{
	if (resourceID != 0)
	{
		App->moduleResources->GetResource(resourceID)->instances--;
	}
}

void Component::SetActive(bool set)
{
	active = set;
}

bool Component::IsActive() const
{
	return active;
}

Component::Type Component::GetType() const
{
	return type;
}

void Component::OnUpdateTransform(const float4x4& global, const float4x4& parent_global)
{

}

void Component::Save()
{

}

void Component::Load()
{

}

void Component::SetResource(Resource* resource)
{

	Resource* oldResource = GetResource();
	if (oldResource != nullptr)
		oldResource->instances--;

	if (resource != nullptr)
	{
		resourceID = resource->GetID();
		resource->instances++;
	}
}

void Component::SetResource(unsigned long long id)
{
	//TODO: find resource by ID
	Resource* oldResource = GetResource();
	if (oldResource != nullptr)
		oldResource->instances--;

	resourceID = id;
	Resource* newResource = GetResource();
	newResource->instances++;
}


Resource* Component::GetResource()
{
	return App->moduleResources->GetResource(resourceID);
}

const Resource* Component::GetResource() const
{
	return App->moduleResources->GetResource(resourceID);
}