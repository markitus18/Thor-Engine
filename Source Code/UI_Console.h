#ifndef __UI_CONSOLE_H__
#define __UI_CONSOLE_H__

#include "ImGui\imgui.h"

class UI_Console
{
private:
	ImGuiTextBuffer buffer;
	bool scrollToBottom;

public:
	bool active = true;

public:

	UI_Console();
	~UI_Console();

	void ClearLog();
	void AddLog(const char* input);

	void Draw(const char* title, bool* p_open);
};

#endif //__UI_CONSOLE_H__
