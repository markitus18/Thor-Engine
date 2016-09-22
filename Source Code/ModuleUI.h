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
	void UpdateFPSData(int fps);

private:

	void ShowAboutWindow();
	void ShowTestWindow();
	void ShowSettingsWindow();

	void InitFPSData();

private:
	char tmp_appName[26] = "Not working yet";
	char tmp_orgName[26] = "Not working yet";

	//Active window controllers
	bool show_About_window = false;
	bool show_Demo_window = false;
	bool show_Test_window = false;
	bool show_Settings_window = true;

	//FPS data saving
	float FPS_data [40];


	ImVec4 test_color;
	UI_Console* console = NULL;
};

#endif
