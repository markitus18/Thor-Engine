#include "Resource.h"

Resource::Resource(Type type) : type(type)
{

}

Resource::~Resource()
{

}

Resource::Type Resource::GetType() const
{
	return type;
}