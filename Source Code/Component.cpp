#include "Component.h"

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