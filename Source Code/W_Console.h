#ifndef __W_CONSOLE_H__
#define __W_CONSOLE_H__

#include "Window.h"

#include <vector>

class W_Console : public Window
{
public:
	W_Console(M_Editor* editor);
	~W_Console() { ClearLog(); }

	void Draw() override;

	void ClearLog();
	void AddLog(const char* input);

private:
	std::vector<char*>	items;
	bool			scrollToBottom;
};

#endif // !__W_INSPECTOR_H__

