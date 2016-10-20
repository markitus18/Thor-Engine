#ifndef __ModuleEditor_H__
#define __ModuleEditor_H__

#include "Module.h"
#include "ImGui\imgui.h"

class PanelConsole;
class PanelHierarchy;
class PanelInspector;
class PanelConfiguration;
class PanelButtons;

class ModuleEditor : public Module
{
public:

	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

	void Log(const char* input);
	void GetEvent(SDL_Event* event);
	void UpdateFPSData(int fps, int ms);

	void OnResize(int screen_width, int screen_height);
private:

	void DrawPanels();
	void ShowAboutWindow();

public:
	//Panels
	PanelHierarchy*		panelHierarchy = NULL;
	PanelConsole*		panelConsole = NULL;
	PanelInspector*		panelInspector = NULL;
	PanelConfiguration* panelConfiguration = NULL;
	PanelButtons*		panelButtons = NULL;

	bool shaded = true;
	bool wireframe = false;
private:

	//Active window controllers
	bool show_About_window = false;
	bool show_Demo_window = false;
};

#endif
