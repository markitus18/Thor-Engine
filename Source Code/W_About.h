#ifndef __W_ABOUT_H__
#define __W_ABOUT_H__

#include "Window.h"

class W_About : public Window
{
public:
	W_About(M_Editor* editor);
	~W_About() {  }

	void Draw() override;
};

#endif // !__W_ABOUT_H__

