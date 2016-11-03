#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "parson\parson.h"

//http://kgabis.github.io/parson/

struct json_object_t;
typedef struct json_object_t JSON_Object;

struct json_value_t;
typedef struct json_value_t  JSON_Value;

struct json_array_t;
typedef struct json_array_t  JSON_Array;

class Config
{
	/* Brief parson explanation
	-	Save a JSON file into a buffer then create a new file with fileSystem:
		JSON_Status json_serialize_to_buffer(const JSON_Value *value, char *buf, size_t buf_size_in_bytes);
	-	JSON nodes are called objects
	-	Append attributes to object: json_object_set_[type of attribute]. Attribute types are bool, number, string and value (any attribute or an object)
	-	Get values from a node: same function as append, but with "get"
	*/
public:
	Config();					//Contructor used for data append
	Config(const char* buffer); //Constructor used for data read

	void Release();				//Freeing JSON_Value data

	//Append attributes
	void SetNumber(const char* name, double data);
	void SetString(const char* name, char* data);
	void SetBool(const char* name, bool data);
	Config SetNode(const char* name);


private:
	JSON_Value* root_value; //Only used for file root
	JSON_Object* node;
};

#endif //__CONFIG_H__
