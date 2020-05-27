#ifndef __W_HIERARCHY_H__
#define __W_HIERARCHY_H__

#include "Window.h"
#include "TreeDisplay.h"

class W_Hierarchy : public Window, public TreeDisplay
{
public:
	W_Hierarchy(M_Editor* editor);
	~W_Hierarchy() {}

	void Draw() override;
};

#endif
