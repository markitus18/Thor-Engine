#ifndef __M_EDITOR_H__
#define __M_EDITOR_H__

#include "Module.h"

#include "ResourceHandle.h"

#include <string>
#include <vector>
#include "MathGeoLib\src\Algorithm\Random\LCG.h"

class Config;
class GameObject;
class TreeNode;

class WindowFrame;
class Window;
class R_Folder;

union SDL_Event;
struct ImGuiWindowClass;

class M_Editor : public Module
{
public:
	M_Editor(bool start_enabled = true);
	~M_Editor();

	bool Init(Config& config) override;
	bool Start() override;
	update_status PreUpdate() override;
	bool CleanUp() override;

	void Draw();

	void Log(const char* input);
	void ProcessSDLEvent(const SDL_Event& event);
	void UpdateFPSData(int fps, int ms);

	void OnResize(int screen_width, int screen_height);

	inline bool UsingKeyboard() const { return using_keyboard; }
	inline bool UsingMouse() const { return using_mouse; }

	bool OpenWindowFromResource(uint64 resourceID, uint64 forceWindowID = 0);

	void RequestWindowClose(WindowFrame* windowFrame);
	void ShowWindowCloseConfirmation();
	void CloseWindow(WindowFrame* windowFrame); //Warning: do not call in the middle of the editor drawing frame!
	void OnUserExitRequest();

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

	void LoadMap(const char* path, bool tmp = false);
	void ResetScene();

	void OnRemoveGameObject(GameObject* gameObject) override;

	void OpenFileNameWindow();
	void ShowFileNameWindow();

	WindowFrame* GetWindowFrame(const char* name) const;
	WindowFrame* GetWindowFromSDLID(uint32_t SDLWindowID) const;
	const char* GetCurrentExplorerDirectory() const;

private:
	void LoadLayout_Default(WindowFrame* windowFrame);
	void LoadLayout(WindowFrame* windowFrame, const char* layout = "Default");
	void SaveEditorState();
	bool TryLoadEditorState();
	bool IsWindowLayoutSaved(WindowFrame* windowFrame) const;

	bool DoesAnyWindowNeedSave() const;

	//imgui.ini parsing functions
	void ClearWindowFromIni(WindowFrame* windowFrame);
	bool Ini_FindNextEntry(char*& line, char*& line_end, char*& buffer_end) const;
	bool Ini_FindNextDockEntry(char*& line, char*& line_end, char*& buffer_end) const;
	
public:
	std::vector<TreeNode*> selectedGameObjects;
	std::vector<TreeNode*> toSelectGOs;
	std::vector<TreeNode*> toDragGOs;
	std::vector<TreeNode*> toUnselectGOs;

	ResourceHandle<Resource> hSelectedResource;
	ResourceHandle<R_Folder> hExplorerFolder;

	TreeNode* lastSelected = nullptr;
	bool dragging = false;

	bool showDebugInfo = false;

	bool show_Demo_window = false;
	bool show_fileName_window = false;
	
	// Temporary set to public
	WindowFrame* windowPendingDelete = nullptr;

private:
	//All currently active window frames
	std::vector<WindowFrame*> windowFrames;

	ImGuiWindowClass* frameWindowClass = nullptr;
	ImGuiWindowClass* normalWindowClass = nullptr;

	char fileName[50];

	bool using_keyboard;
	bool using_mouse;

	float mainWindowPositionY = 0;
	float toolbarSizeY = 30;

	bool userRequestedExit = false;

	LCG random; //TODO: Avoid including everywhere. Math namespace?
};

#endif //!__M_EDITOR_H__
