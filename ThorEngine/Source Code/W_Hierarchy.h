#ifndef __W_HIERARCHY_H__
#define __W_HIERARCHY_H__

#include "Window.h"
#include "TreeDisplay.h"

struct ImGuiWindowClass;

class W_Hierarchy : public Window, public TreeDisplay
{
public:
	W_Hierarchy(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID);
	~W_Hierarchy() {}

	void Draw() override;
	static inline const char* GetName() { return "Hierarchy"; };

};

#endif
