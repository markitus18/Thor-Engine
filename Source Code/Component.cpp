#include "Component.h"

Component::Component(Type type, GameObject* _gameObject) : type(type)
{
	//Removed temporaly. Is it really necessary?
	//gameObject = _gameObject;
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