#include "Window.h"

struct ImGuiWindowClass;

class W_MainToolbar : public Window
{
public:
	W_MainToolbar(M_Editor* editor, ImGuiWindowClass* windowClass);
	~W_MainToolbar() {  }

	void Draw() override;
	static inline const char* GetName() { return "Toolbar"; };

};