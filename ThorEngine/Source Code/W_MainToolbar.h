#include "Window.h"

struct ImGuiWindowClass;

class W_MainToolbar : public Window
{
public:
	W_MainToolbar(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID);
	~W_MainToolbar() {  }

	void Draw() override;
	static inline const char* GetName() { return "Toolbar"; };

};