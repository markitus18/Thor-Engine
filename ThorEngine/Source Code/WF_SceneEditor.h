#ifndef __WF_MAIN_WINDOW_H__
#define __WF_MAIN_WINDOW_H__

#include "WindowFrame.h"

class M_Editor;

class W_Scene;
class W_Console;
class W_Explorer;
class W_EngineConfig;
class W_Resources;
class W_Inspector;
class W_Hierarchy;
class W_About;
class W_MainToolbar;

struct ImGuiWindowClass;

class WF_SceneEditor : public WindowFrame
{
public:
	WF_SceneEditor(M_Editor* editor, ImGuiWindowClass* frameWindowClass, ImGuiWindowClass* windowClass, int ID);
	~WF_SceneEditor();

	void SetResource(uint64 resourceID) override;

	void LoadLayout_Default(ImGuiID mainDockID) override;
	static inline const char* GetName() { return "SceneEditor"; };

private:
	void MenuBar_File() override;
	void MenuBar_Custom() override;
	void MenuBar_Development() override;

	//Specific window class for the explorer smallest windows
	ImGuiWindowClass* explorerWindowClass = nullptr;
};

#endif // !__WF_MAIN_WINDOW_H

