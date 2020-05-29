#ifndef __W_ABOUT_H__
#define __W_ABOUT_H__

#include "Window.h"

struct ImGuiWindowClass;

class W_About : public Window
{
public:
	W_About(M_Editor* editor, ImGuiWindowClass* windowClass);
	~W_About() {  }

	void Draw() override;

	static inline const char* GetName() { return "About"; };
};

#endif // !__W_ABOUT_H__

