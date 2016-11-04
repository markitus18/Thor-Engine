#include "Config.h"

//Contructor used for data append
Config::Config(bool alloc)
{
	if (alloc)
	{
		root_value = json_value_init_object();
		node = json_value_get_object(root_value);
	}
}

//Constructor used for data read
Config::Config(const char* buffer)
{
	root_value = json_parse_string(buffer);
	if (root_value)
	{
		node = json_value_get_object(root_value);
	}
}

Config::~Config()
{
	if (root_value)
	{
		json_value_free(root_value);
	}
}

//Fills a buffer, returns its size
uint Config::Serialize(char** buffer)
{
	size_t size = json_serialization_size(root_value);
	*buffer = new char[size];
	json_serialize_to_buffer(root_value, *buffer, size);
	return size;
}

bool Config::NodeExists()
{
	//Casting (bool)root_value could cause problems
	return root_value != nullptr;
}

void Config::SetNumber(const char* name, double data)
{
	json_object_set_number(node, name, data);
}

void Config::SetString(const char* name, char* data)
{
	json_object_set_string(node, name, data);
}

void Config::SetBool(const char* name, bool data)
{
	json_object_set_boolean(node, name, data);
}

Config Config::SetNode(const char* name)
{
	///	Code reference:
	/*
		json_object_set_value(root, list_modules[i]->name.c_str(), json_value_init_object());
		list_modules[i]->Save(json_object_get_object(root, list_modules[i]->name.c_str()));
	*/
	 
	Config config(false);
	json_object_set_value(node, name, json_value_init_object());
	config.node = json_object_get_object(node, name);
	return config;
}

//Get attributes --------------
int Config::GetNumber(const char* name) const
{
	return json_object_get_number(node, name);
}

std::string Config::GetString(const char* name) const
{
	return json_object_get_string(node, name);
}

bool Config::GetBool(const char* name) const
{
	return json_object_get_boolean(node, name);
}

Config Config::GetNode(const char* name) const
{
	Config config(false);
	config.node = json_object_get_object(node, name);
	return config;
}

//Endof Get attributes---------