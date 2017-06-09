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
	KEY_UP,
	KEY_CLICK, //For mouse handling, if the mouse has been pressed and released, but not moved
};

class Config;

class M_Input : public Module
{
public:
	
	M_Input(bool start_enabled = true);
	~M_Input();

	bool Init(Config& config);
	update_status PreUpdate(float dt);
	bool CleanUp();

	void SetMouseX(int x);
	void SetMouseY(int y);

	void InfiniteHorizontal();

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

	bool LastClickMoved() const
	{
		if (GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
		{
			if (last_mouse_click_x != mouse_x)
				return true;
			if (last_mouse_click_y != mouse_y)
				return true;
			return false;
			return (last_mouse_click_x != mouse_x || last_mouse_click_y != mouse_y);
		}
		return false;
	}
private:
	void ResetImGuiDrag();

public:
	//Dirty variable to tell ImGui not to drag
	bool ignoreDrag = false;

private:
	KEY_STATE* keyboard = nullptr;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];
	int mouse_x  = 0;
	int mouse_y = 0;
	int mouse_z = 0;
	int mouse_motion_x = 0;
	int mouse_motion_y = 0;

	int last_mouse_click_x = 0;
	int last_mouse_click_y = 0;

	//TODO: change "ifinite mouse" functionality in a more polite way
	int last_mouse_swap = 0;
	bool infiniteHorizontal = false;
	int ignoreDragCount = 0;

};

#endif //__MODULE_INPUT_H__