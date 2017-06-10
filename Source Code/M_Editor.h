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
class P_Resources;
class P_Explorer;

class Config;
struct PathNode;
class GameObject;
class Resource;

class M_Editor : public Module
{
public:

	M_Editor(bool start_enabled = true);
	~M_Editor();

	bool Init(Config& config);
	bool Start();
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

	//Selection --------------------
	void SelectSingle(GameObject* gameObject, bool openTree = true);
	void AddSelect(GameObject* gameObject, bool openTree = true);
	void AddToSelect(GameObject* gameObject);
	void UnselectSingle(GameObject* gameObject);
	void UnselectAll();
	void UnselectGameObjects();
	void UnselectResources();
	void DeleteSelected();
	//Endof Selection------------------------------------

	void LoadScene(Config& root, bool tmp = false);
	void ResetScene();

	void OnRemoveGameObject(GameObject* gameObject) override;

private:
	void DrawPanels();
	void DisplayScenesWindows(const PathNode& folder);
	//void DisplayFolderTree();

	void ShowAboutWindow();
	void ShowPlayWindow();

	void OpenFileNameWindow();
	void ShowFileNameWindow();

	//TODO: move into private
public:
	//Panels
	P_Hierarchy*		hierarchy = nullptr;
	P_Console*			console = nullptr;
	P_Inspector*		inspector = nullptr;
	P_Configuration*	configuration = nullptr;
	P_Buttons*			buttons = nullptr;
	P_Resources*		resources = nullptr;
	P_Explorer*			explorer = nullptr;

	ImVec2 playWindow;

	bool shaded = true;
	bool wireframe = false;

	std::vector<GameObject*> selectedGameObjects;
	std::vector<GameObject*> toSelectGOs;
	std::vector<GameObject*> toDragGOs;

	std::vector<Resource*> selectedResources;

	GameObject* lastSelected = nullptr;

private:

	//Active window controllers
	bool show_About_window = false;
	bool show_Demo_window = false;
	bool show_fileName_window = false;

	char fileName[50];

	std::vector<std::string> sceneList;

	bool using_keyboard;
	bool using_mouse;

	bool dragging = true;
};

#endif
