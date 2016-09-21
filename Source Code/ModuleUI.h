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
	void GetEvent(SDL_Event* event);

private:

	void ShowAboutWindow();
	void ShowTestWindow();
	void ShowSettingsWindow();

private:
	char* buf;

	//Active window controllers
	bool show_second_window = false;
	bool show_About_window = false;
	bool show_Demo_window = false;
	bool show_Test_window = false;
	bool show_Settings_window = true;

	ImVec4 test_color;
	UI_Console* console = NULL;
};

#endif
