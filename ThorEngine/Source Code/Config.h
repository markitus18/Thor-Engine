#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "parson\parson.h"
#include <string>
#include "Globals.h"
#include <vector>
#include "MathGeoLib\src\MathGeoLib.h"

//http://kgabis.github.io/parson/

struct json_object_t;
typedef struct json_object_t JSON_Object;

struct json_value_t;
typedef struct json_value_t  JSON_Value;

struct json_array_t;
typedef struct json_array_t  JSON_Array;

class Config_Array;
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
	friend class Config_Array;

public:
	Config();						//Contructor used for new files
	Config(const char* buffer);		//Constructor used for data read
	~Config();						//Free data if initialized
private:
	Config(JSON_Object* obj, bool isSaving);		//Constructor used for node append

public:
	void Set(const char* buffer);
	uint Serialize(char** buffer) const;	//Returns a filled buffer
	bool NodeExists();
	void Release();

	//Append attributes -----------
	void Serialize(const char* name, int& data, int default = 0);
	void Serialize(const char* name, uint& data, uint default = 0);
	void Serialize(const char* name, uint64& data, uint64 default = 0);
	void Serialize(const char* name, float& data, float default = .0f);

	void Serialize(const char* name, bool& data, bool default = true);
	void Serialize(const char* name, std::string& data, const char* default = "");

	void SerializeArray(const char* name, int* data, uint size);
	void SerializeArray(const char* name, uint64* data, uint size);
	void SerializeArray(const char* name, float* data, uint size);

	template <class T>
	void SerializeVector(const char* name, std::vector<T>& vector)
	{
		Config_Array arr;
		if (isSaving)
		{
			arr = SetArray(name);
		}
		else
		{
			arr = GetArray(name);
			vector.resize(arr.GetSize());
		}
		for (uint i = 0; i < vector.size(); ++i)
		{
			vector[i].Serialize(arr.GetNode(i));
		}
	}

	void SetBool(const char* name, bool data);
	Config_Array SetArray(const char* name);
	Config SetNode(const char* name);
	//Endof append attributes------

	//Get attributes --------------
	bool GetBool(const char* name, bool default = true) const;
	Config_Array GetArray(const char* name) const;
	Config GetNode(const char* name) const;
	//Endof Get attributes---------
	
public:
	bool isSaving;

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

	bool IsValid();

	//Append attributes ------------
	void AddNumber(double number);
	void AddString(char* string);
	void AddBool(bool boolean);
	void AddFloat3(const float3& data);
	void AddFloat4(const float4& data);

	void AddQuat(const Quat& data);
	Config AddNode();
	//Endof append attributes-------

	//Get attributes ---------------
	double GetNumber(uint index, double default = 0) const;
	const char* GetString(uint index, const char* default = "") const;
	bool GetBool(uint index, bool default = true) const;
	float3 GetFloat3(uint index, float3 default = float3::zero) const; //Index is based on float3 not on single data!
	float4 GetFloat4(uint index, float4 default = float4::zero) const; //Index is based on float4 not on single data!
	Quat GetQuat(uint index, Quat  default = Quat::identity) const;
	void FillVectorNumber(std::vector<double>& vector) const;
	void FillVectorString(std::vector<char*>& vector) const;
	void FillVectorBoool(std::vector<bool>& vector) const;
	Config GetNode(uint index) const;
	uint GetSize() const;
	//Endof Get attributes----------

private:
	JSON_Array* arr;
	uint size = 0;
};

#endif //__CONFIG_H__
