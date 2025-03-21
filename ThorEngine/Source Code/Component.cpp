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

void Component::Serialize(Config& config)
{
	int intType = static_cast<int>(type);
	config.Serialize("Component Type", intType);
}
