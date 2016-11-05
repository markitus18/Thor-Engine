#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "parson\parson.h"
#include <string>
#include "Globals.h"

//http://kgabis.github.io/parson/

struct json_object_t;
typedef struct json_object_t JSON_Object;

struct json_value_t;
typedef struct json_value_t  JSON_Value;

struct json_array_t;
typedef struct json_array_t  JSON_Array;

class Config
{
	///Brief parson explanation
	/* 
	-	Save a JSON file into a buffer then create a new file with fileSystem:
		JSON_Status json_serialize_to_buffer(const JSON_Value *value, char *buf, size_t buf_size_in_bytes);
	-	JSON nodes are called objects
	-	Append attributes to object: json_object_set_[type of attribute]. Attribute types are bool, number, string and value (any attribute or an object)
	-	Get values from a node: same function as append, but with "get"
	*/

public:
	Config(bool alloc = true);	//Contructor used for data append
	Config(const char* buffer); //Constructor used for data read
	~Config();					//Free data if initialized

	uint Serialize(char** buffer);	//Returns a filled buffer
	bool NodeExists();

	//Append attributes -----------
	void SetNumber(const char* name, double data);
	void SetString(const char* name, char* data);
	void SetBool(const char* name, bool data);
	Config_Array SetArray(const char* name);
	Config SetNode(const char* name);
	//Endof append attributes------

	//Get attributes --------------
	double GetNumber(const char* name, double default) const;
	std::string GetString(const char* name, const char* default) const;
	bool GetBool(const char* name, bool default) const;
	Config GetNode(const char* name) const;
	//Endof Get attributes---------
	
private:
	JSON_Value* root_value = nullptr; //Only used for file root
	JSON_Object* node = nullptr;
};

class Config_Array
{
public:
	//Contructor only to be called from Config, it would cause mem leak
	Config_Array();
	Config_Array(JSON_Array* arr);

	//Append attributes ------------
	void AddNumber(int number);
	void AddString(char* string);
	void AddBool(bool boolean);
	//Endof append attributes-------

	//Get attributes ---------------
	double GetNumber(int index, double default);
	const char* GetString(int index, const char* default);
	bool GetBool(int index, bool default);
	//Endof Get attributes----------

private:
	JSON_Array* arr;
	uint size;
};
#endif //__CONFIG_H__
