#ifndef __ModuleEditor_H__
#define __ModuleEditor_H__

#include "Module.h"
#include "ImGui\imgui.h"

class PanelConsole;
class PanelHierarchy;
class PanelInspector;
class PanelConfiguration;
class PanelButtons;

class Config;

class ModuleEditor : public Module
{
public:

	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Init(Config& config);
	update_status PreUpdate(float dt);
	bool CleanUp();

	void Draw();

	void Log(const char* input);
	void GetEvent(SDL_Event* event);
	void UpdateFPSData(int fps, int ms);

	void OnResize(int screen_width, int screen_height);

	bool UsingKeyboard() const;
	bool UsingMouse() const;

	//Timer management -------------------
	uint AddTimer(const char* text, const char* tag);
	void StartTimer(uint index);
	void ReadTimer(uint index);
	void StopTimer(uint index);
	//------------------------------------

	void LoadScene(Config& root);
	void ResetScene();

private:
	void DrawPanels();
	void ShowAboutWindow();
	void ShowFileNameWindow();

	void StartFileNameWindow();
	//TODO: move into private
public:
	//Panels
	PanelHierarchy*		panelHierarchy = nullptr;
	PanelConsole*		panelConsole = nullptr;
	PanelInspector*		panelInspector = nullptr;
	PanelConfiguration* panelConfiguration = nullptr;
	PanelButtons*		panelButtons = nullptr;

	bool shaded = true;
	bool wireframe = false;
private:

	//Active window controllers
	bool show_About_window = false;
	bool show_Demo_window = false;
	bool show_fileName_window = false;

	char fileName[50];

	bool using_keyboard;
	bool using_mouse;
};

#endif
