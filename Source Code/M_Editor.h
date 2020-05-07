#ifndef __M_EDITOR_H__
#define __M_EDITOR_H__

#include "Module.h"
#include "ImGui\imgui.h" //TODO: try to forward declare ImVec2
#include "SDL/include/SDL_events.h" //TODO: try to forward declare
#include <string>
#include <vector>

class P_Buttons;

class Config;
class GameObject;
class Resource;
class TreeNode;

class Dock;
class DWindow;
class W_Scene;
class W_Console;
class W_Explorer;
class W_EngineConfig;
class W_ParticleEditor;
class W_Resources;
class W_Inspector;

class M_Editor : public Module
{
public:

	M_Editor(bool start_enabled = true);
	~M_Editor();

	bool Init(Config& config) override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	bool CleanUp() override;

	void CreateWindows();
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

	void SaveConfig(Config& config) const override;
	void LoadConfig(Config& config) override;

	void LoadScene(Config& root, bool tmp = false) override;
	void ResetScene();

	void OnRemoveGameObject(GameObject* gameObject) override;

private:
	void DrawPanels();
	//void DisplayFolderTree();

	void ShowAboutWindow();
	void ShowPlayWindow();

	void OpenFileNameWindow();
	void ShowFileNameWindow();

	//TODO: move into private
public:

	//Windows system
	std::vector<Dock*> docks;
	std::vector<DWindow*> windows;

	Dock* focusedDock = nullptr;

	W_Console* w_console = nullptr;
	W_EngineConfig* w_econfig = nullptr;
	W_Explorer* w_explorer = nullptr;
	W_Scene* w_scene = nullptr;
	W_ParticleEditor* w_particles = nullptr;
	W_Inspector* w_inspector = nullptr;
	W_Resources* w_resources = nullptr;
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

	bool showDebugInfo = false;

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
