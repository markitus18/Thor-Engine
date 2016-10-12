#ifndef __UI_CONSOLE_H__
#define __UI_CONSOLE_H__

#include "ImGui\imgui.h"
#include "Panel.h"

class PanelConsole : public Panel
{
private:
	ImVector<char*>	items;
	bool			scrollToBottom;

public:

	PanelConsole();
	~PanelConsole();

	void ClearLog();
	void AddLog(const char* input);

	void Draw(ImGuiWindowFlags flags);

	void UpdatePosition(int, int);
};

#endif //__UI_CONSOLE_H__
