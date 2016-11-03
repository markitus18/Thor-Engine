#ifndef __MODULE_H__
#define __MODULE_H__

#include "Globals.h"
#include <string>
#include "parson/parson.h"

class Application;
struct PhysBody3D;

class Module
{
private :
	bool enabled;

public:
	std::string name;

	Module(const char* name, bool start_enabled = true) : name(name)
	{
		enabled = start_enabled;
	}

	virtual ~Module()
	{}

	bool IsEnabled() const
	{
		return enabled;
	}

	void Enable()
	{
		if (enabled == false)
		{
			enabled = true;
			Start();
		}
	}

	bool Disable()
	{
		bool ret = true;
		if (enabled == true)
		{
			enabled = false;
			ret = CleanUp();
		}
		return ret;
	}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void Save(JSON_Object* root)
	{}

	virtual void Load(JSON_Object* root)
	{}

};

#endif //__MODULE_H__