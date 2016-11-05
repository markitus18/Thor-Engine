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

Config_Array Config::SetArray(const char* name)
{
	//TODO init array, add it, and return it
	//JSON_Vjson_value_init_array();
	return Config_Array();
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
double Config::GetNumber(const char* name, double default) const
{
	if (json_object_has_value_of_type(node, name, JSONNumber))
		return json_object_get_number(node, name);
	return default;
}

std::string Config::GetString(const char* name, const char* default) const
{
	if (json_object_has_value_of_type(node, name, JSONString))
		return json_object_get_string(node, name);
	return default;
}

bool Config::GetBool(const char* name, bool default) const
{
	if (json_object_has_value_of_type(node, name, JSONBoolean))
		return json_object_get_boolean(node, name);
	return default;
}

Config Config::GetNode(const char* name) const
{
	Config config(false);
	config.node = json_object_get_object(node, name);
	return config;
}
//Endof Get attributes---------

Config_Array::Config_Array()
{
	arr = json_value_get_array(json_value_init_array());
}

Config_Array::Config_Array(JSON_Array* arr) : arr(arr)
{
	size = json_array_get_count(arr);
}

//Append attributes ------------
void Config_Array::AddNumber(int number)
{
	json_array_append_number(arr, number);
	size++;
}

void Config_Array::AddString(char* string)
{
	json_array_append_string(arr, string);
	size++;
}

void Config_Array::AddBool(bool boolean)
{
	json_array_append_boolean(arr, boolean);
	size++;
}
//Endof append attributes-------

//Get attributes ---------------
double Config_Array::GetNumber(int index, double default)
{
	if (index < size)
		return json_array_get_number(arr, index);
	else
	{
		LOG("[Warning] JSON Array: Index out of size");
		return 0;
	}
}

const char* Config_Array::GetString(int index, const char* default)
{
	if (index < size)
		return json_array_get_string(arr, index);
	else
	{
		LOG("[Warning] JSON Array: Index out of size");
		return nullptr;
	}
}

bool Config_Array::GetBool(int index, bool default)
{
	if (index < size)
		return json_array_get_boolean(arr, index);
	else
	{
		LOG("[Warning] JSON Array: Index out of size");
		return false;
	}
}
//Endof Get attributes----------