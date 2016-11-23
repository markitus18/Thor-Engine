#ifndef __UI_CONSOLE_H__
#define __UI_CONSOLE_H__

#include "ImGui\imgui.h"
#include "Panel.h"

class P_Console : public Panel
{
private:
	ImVector<char*>	items;
	bool			scrollToBottom;

public:

	P_Console();
	~P_Console();

	void ClearLog();
	void AddLog(const char* input);

	void Draw(ImGuiWindowFlags flags);

	void UpdatePosition(int, int);
};

#endif //__UI_CONSOLE_H__
