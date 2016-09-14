#ifndef __MODULEUI_H__
#define __MODULEUI_H__

#include "Module.h"

class ModuleUI : public Module
{
public:

	ModuleUI(Application* app, bool start_enabled = true);
	~ModuleUI();

	bool Init();
	update_status Update(float dt);
	bool CleanUp();

private:
	bool show_second_window = false;

};

#endif
