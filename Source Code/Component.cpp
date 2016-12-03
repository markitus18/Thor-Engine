#include "Component.h"
#include "Resource.h"

Component::Component(Type type, const GameObject* gameObject) : type(type)
{
	this->gameObject = gameObject;
}

Component::~Component()
{

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
	if (this->resource)
	{
		this->resource->instances--;
	}
	if (resource)
	{
		this->resource = resource;
		resourceID = resource->GetID();
		resource->instances++;
	}
}

void Component::SetResource(unsigned long long id)
{
	//TODO: find resource by ID
	resourceID = id;
}


Resource* Component::GetResource()
{
	return resource;
}

const Resource* Component::GetResource() const
{
	return resource;
}