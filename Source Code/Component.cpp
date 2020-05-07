#include "Component.h"
#include "Resource.h"
#include "Engine.h"
#include "M_Resources.h"
#include "Config.h"

Component::Component(Type type, GameObject* gameObject, bool hasResource) : type(type), hasResource(hasResource), gameObject(gameObject)
{
}

Component::~Component()
{
	if (resourceID != 0)
	{
		Engine->moduleResources->GetResource(resourceID)->instances--;
	}
}

void Component::SetActive(bool set)
{
	active = set;
}

void Component::OnUpdateTransform(const float4x4& global, const float4x4& parent_global)
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

void Component::SetResource(unsigned long long id, bool load)
{
	if (load)
		if (Resource* oldResource = GetResource())
			oldResource->instances--;

	resourceID = id;
	if (load)
	{
		if (Resource* newResource = GetResource())
			newResource->instances++;
	}
}


Resource* Component::GetResource()
{
	return Engine->moduleResources->GetResource(resourceID);
}

const Resource* Component::GetResource() const
{
	return Engine->moduleResources->GetResource(resourceID);
}