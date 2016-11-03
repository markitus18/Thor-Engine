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

void Config::Release()
{
	json_value_free(root_value);
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
	Config newConfig;
	//json_object_set_value(node, name, newConfig.node);
	return newConfig;
}
