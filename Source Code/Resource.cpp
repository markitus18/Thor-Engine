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

unsigned long long Resource::GetID() const
{
	return ID;
}

const char* Resource::GetResourceFile() const
{
	return resource_file.c_str();
}