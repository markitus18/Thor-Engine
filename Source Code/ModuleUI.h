#ifndef __MODULEUI_H__
#define __MODULEUI_H__

#include "Module.h"
#include "ImGui\imgui.h"

class UI_Console;

class ModuleUI : public Module
{
public:

	ModuleUI(Application* app, bool start_enabled = true);
	~ModuleUI();

	bool Init();
	update_status Update(float dt);
	bool CleanUp();

	void Log(const char* input);

private:
	bool show_second_window = false;
	ImVec4 test_color;
	UI_Console* console = NULL;
};

#endif
