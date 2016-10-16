#ifndef __MODULE_INPUT_H__
#define __MODULE_INPUT_H__

#include "Module.h"
#include "Globals.h"

#define MAX_MOUSE_BUTTONS 5

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:
	
	ModuleInput(Application* app, bool start_enabled = true);
	~ModuleInput();

	bool Init();
	update_status PreUpdate(float dt);
	bool CleanUp();

	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButton(int id) const
	{
		return mouse_buttons[id];
	}

	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouse_motion_x;
	}

	int GetMouseYMotion() const
	{
		return mouse_motion_y;
	}

private:
	KEY_STATE* keyboard = NULL;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];
	int mouse_x  = 0;
	int mouse_y = 0;
	int mouse_z = 0;
	int mouse_motion_x = 0;
	int mouse_motion_y = 0;
};

#endif //__MODULE_INPUT_H__