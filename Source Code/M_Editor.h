#ifndef __M_EDITOR_H__
#define __M_EDITOR_H__

#include "Module.h"
#include "ImGui\imgui.h" //TODO: try to forward declare ImVec2
#include "SDL/include/SDL_events.h" //TODO: try to forward declare
#include <string>
#include <vector>

class P_Buttons;

class Config;
struct PathNode;
class GameObject;
class Resource;
class TreeNode;

class Dock;
class W_Console;
class W_Explorer;
class W_EngineConfig;

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
	void SelectSingle(TreeNode* node, bool openTree = true);
	void AddSelect(TreeNode* node, bool openTree = true);
	void AddToSelect(TreeNode* node);
	void UnselectSingle(TreeNode* node);
	void UnselectAll();
	void UnselectGameObjects();
	void UnselectResources();
	void DeleteSelected();

	void FinishDrag(bool drag, bool SelectDrag);
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

	//Windows system
	std::vector<Dock*> docks;
	Dock* focusedDock = nullptr;
	W_Console* w_console = nullptr;
	W_EngineConfig* w_econfig = nullptr;
	W_Explorer* w_explorer = nullptr;

	//Panels
	P_Buttons*			buttons = nullptr;

	ImVec2 playWindow;

	bool shaded = true;
	bool wireframe = false;

	std::vector<TreeNode*> selectedGameObjects;
	std::vector<TreeNode*> toSelectGOs;
	std::vector<TreeNode*> toDragGOs;
	std::vector<TreeNode*> toUnselectGOs;

	std::vector<Resource*> selectedResources;

	TreeNode* lastSelected = nullptr;
	bool dragging = false;
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

#endif //!__M_EDITOR_H__
