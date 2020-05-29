#ifndef __W_CONSOLE_H__
#define __W_CONSOLE_H__

#include "Window.h"

#include <vector>

struct ImGuiWindowClass;

//FIXME: Change name for 'Output'
class W_Console : public Window
{
public:
	W_Console(M_Editor* editor, ImGuiWindowClass* windowClass);
	~W_Console() { ClearLog(); }

	void Draw() override;

	void ClearLog();
	void AddLog(const char* input);

	static inline const char* GetName() { return "Console"; };

private:
	std::vector<char*>	items;
	bool			scrollToBottom;
};

#endif // !__W_INSPECTOR_H__

