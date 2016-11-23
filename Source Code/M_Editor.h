#ifndef __M_Editor_H__
#define __M_Editor_H__

#include "Module.h"
#include "ImGui\imgui.h"
#include <string>
#include <vector>

class P_Console;
class P_Hierarchy;
class P_Inspector;
class P_Configuration;
class P_Buttons;

class Config;

class M_Editor : public Module
{
public:

	M_Editor(Application* app, bool start_enabled = true);
	~M_Editor();

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

	//GameObject stuff -------------------
	void SelectGameObject(GameObject* gameObject, bool selectSingle = true);
	//------------------------------------

	void LoadScene(Config& root);
	void ResetScene();

	void OnRemoveGameObject(GameObject* gameObject) override;

private:
	void DrawPanels();
	void ShowAboutWindow();

	void OpenFileNameWindow();
	void ShowFileNameWindow();

	void DeleteSelectedGameObjects();
	//TODO: move into private
public:
	//Panels
	P_Hierarchy*		panelHierarchy = nullptr;
	P_Console*		panelConsole = nullptr;
	P_Inspector*		panelInspector = nullptr;
	P_Configuration* panelConfiguration = nullptr;
	P_Buttons*		panelButtons = nullptr;

	bool shaded = true;
	bool wireframe = false;
private:

	//Active window controllers
	bool show_About_window = false;
	bool show_Demo_window = false;
	bool show_fileName_window = false;

	char fileName[50];

	std::vector<std::string> sceneList;

	bool using_keyboard;
	bool using_mouse;
};

#endif
