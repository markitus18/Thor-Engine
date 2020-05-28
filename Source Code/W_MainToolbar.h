#include "Window.h"

class W_MainToolbar : public Window
{
public:
	W_MainToolbar(M_Editor* editor);
	~W_MainToolbar() {  }

	void Draw() override;
};