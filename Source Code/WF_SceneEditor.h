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

class WF_SceneEditor : public WindowFrame
{
public:
	WF_SceneEditor(M_Editor* editor);
	~WF_SceneEditor();

	void LoadLayout_ForceDefault(Config& file, ImGuiID mainDockID) override;

private:
	void MenuBar_File() override;
	void MenuBar_Custom() override;
	void MenuBar_Development() override;

private:
	//TODO: Is it worth storing it as a ptr? Saves compilation time but it's stored in the heap
	W_Hierarchy*	w_hierarchy = nullptr;
	W_Console*		w_console = nullptr;
	W_EngineConfig* w_econfig = nullptr;
	W_Explorer*		w_explorer = nullptr;
	W_Scene*		w_scene = nullptr;
	W_Inspector*	w_inspector = nullptr;
	W_Resources*	w_resources = nullptr;
	W_About*		w_about = nullptr;
	W_MainToolbar*	w_toolbar = nullptr;
};

#endif // !__WF_MAIN_WINDOW_H

