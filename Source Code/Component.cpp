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

}

void Component::SetActive(bool set)
{
	active = set;
}

void Component::OnUpdateTransform(const float4x4& global, const float4x4& parent_global)
{

}