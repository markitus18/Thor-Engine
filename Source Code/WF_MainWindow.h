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

class WF_MainWindow : public WindowFrame
{
public:
	WF_MainWindow(M_Editor* editor);
	~WF_MainWindow();

	void DrawMenuBar();

private:
	//TODO: Is it worth storing it as a ptr? Saves compilation time but gets saved in the heap
	W_Hierarchy*	w_hierarchy = nullptr;
	W_Console*		w_console = nullptr;
	W_EngineConfig* w_econfig = nullptr;
	W_Explorer*		w_explorer = nullptr;
	W_Scene*		w_scene = nullptr;
	W_Inspector*	w_inspector = nullptr;
	W_Resources*	w_resources = nullptr;
};

#endif // !__WF_MAIN_WINDOW_H

