#include "Component.h"

Component::Component(Type type, GameObject* _gameObject) : type(type)
{
	gameObject = _gameObject;
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