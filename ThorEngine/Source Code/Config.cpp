#include "Config.h"

//Contructor used for data append
Config::Config()
{
	root_value = json_value_init_object();
	node = json_value_get_object(root_value);
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

Config::Config(JSON_Object* obj) : node(obj)
{

}

Config::~Config()
{
	Release();
}

void Config::Set(const char* buffer)
{
	if (root_value)
		json_value_free(root_value);
	root_value = json_parse_string(buffer);
	if (root_value)
	{
		node = json_value_get_object(root_value);
	}
}

//Fills a buffer, returns its size
uint Config::Serialize(char** buffer) const
{
	size_t size = json_serialization_size_pretty(root_value);
	*buffer = new char[size];
	json_serialize_to_buffer_pretty(root_value, *buffer, size);
	return size;
}

bool Config::NodeExists()
{
	//Casting (bool)root_value could cause problems
	return root_value != nullptr;
}

void Config::Release()
{
	if (root_value)
	{
		json_value_free(root_value);
	}
}

void Config::SetNumber(const char* name, double data)
{
	json_object_set_number(node, name, data);
}

void Config::SetString(const char* name, const char* data)
{
	json_object_set_string(node, name, data);
}

void Config::SetBool(const char* name, bool data)
{
	json_object_set_boolean(node, name, data);
}

Config_Array Config::SetArray(const char* name)
{
	json_object_set_value(node, name, json_value_init_array());
	return Config_Array(json_object_get_array(node, name));
}

Config Config::SetNode(const char* name)
{
	json_object_set_value(node, name, json_value_init_object());
	return Config(json_object_get_object(node, name));
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

Config_Array Config::GetArray(const char* name) const
{
	if (json_object_has_value_of_type(node, name, JSONArray))
		return Config_Array(json_object_get_array(node, name));
	else
	{
		//Careful, if this else is entered we cause a memory leak, but at least
		//program doesn't break and we can see the error
		LOG("[error] Array '%s' not found when loading scene", name);
		return Config_Array();
	}
}

Config Config::GetNode(const char* name) const
{
	return Config(json_object_get_object(node, name));
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
void Config_Array::AddNumber(double number)
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

void Config_Array::AddFloat3(const float3& data)
{
	json_array_append_number(arr, data.x);
	json_array_append_number(arr, data.y);
	json_array_append_number(arr, data.z);
	size += 3;
}

void Config_Array::AddFloat4(const float4& data)
{
	json_array_append_number(arr, data.x);
	json_array_append_number(arr, data.y);
	json_array_append_number(arr, data.z);
	json_array_append_number(arr, data.w);
	size += 4;
}

void Config_Array::AddQuat(const Quat& data)
{
	json_array_append_number(arr, data.x);
	json_array_append_number(arr, data.y);
	json_array_append_number(arr, data.z);
	json_array_append_number(arr, data.w);
	size += 4;
}

Config Config_Array::AddNode()
{
	json_array_append_value(arr, json_value_init_object());
	size++;
	return Config(json_array_get_object(arr, size - 1));
}
//Endof append attributes-------

//Get attributes ---------------
double Config_Array::GetNumber(uint index, double default) const
{
	if (index < size)
		return json_array_get_number(arr, index);
	else
	{
		LOG("[Warning] JSON Array: Index out of size");
		return 0;
	}
}

const char* Config_Array::GetString(uint index, const char* default) const
{
	if (index < size)
		return json_array_get_string(arr, index);
	else
	{
		LOG("[Warning] JSON Array: Index out of size");
		return default;
	}
}

float3 Config_Array::GetFloat3(uint index, float3 default) const
{
	index *= 3;
	float3 ret = default;

	ret.x = GetNumber(index + 0, ret.x);
	ret.y = GetNumber(index + 1, ret.y);
	ret.z = GetNumber(index + 2, ret.z);

	return ret;
}

float4 Config_Array::GetFloat4(uint index, float4 default) const
{
	index *= 4;
	float4 ret = default;

	ret.x = GetNumber(index + 0, ret.x);
	ret.y = GetNumber(index + 1, ret.y);
	ret.z = GetNumber(index + 2, ret.z);
	ret.w = GetNumber(index + 3, ret.z);

	return ret;
}

Quat Config_Array::GetQuat(uint index, Quat  default) const
{
	index *= 4;
	Quat ret = default;

	ret.x = GetNumber(index + 0, ret.x);
	ret.y = GetNumber(index + 1, ret.y);
	ret.z = GetNumber(index + 2, ret.z);
	ret.w = GetNumber(index + 3, ret.w);

	return ret;
}

bool Config_Array::GetBool(uint index, bool default) const
{
	if (index < size)
		return json_array_get_boolean(arr, index);
	else
	{
		LOG("[Warning] JSON Array: Index out of size");
		return default;
	}
}

void Config_Array::FillVectorNumber(std::vector<double>& vector) const
{
	for (uint i = 0; i < size; i++)
	{
		vector.push_back(GetNumber(i));
	}
}

void Config_Array::FillVectorString(std::vector<char*>& vector) const
{
	for (uint i = 0; i < size; i++)
	{
		vector.push_back((char*)GetString(i));
	}
}

void Config_Array::FillVectorBoool(std::vector<bool>& vector) const
{
	for (uint i = 0; i < size; i++)
	{
		vector.push_back(GetBool(i));
	}
}

Config Config_Array::GetNode(uint index) const
{
	return Config(json_array_get_object(arr, index));
}

uint Config_Array::GetSize() const
{
	return size;
}
//Endof Get attributes----------