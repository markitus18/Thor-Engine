#ifndef __W_HIERARCHY_H__
#define __W_HIERARCHY_H__

#include "DWindow.h"
#include "TreeDisplay.h"

class W_Hierarchy : public DWindow, public TreeDisplay
{
public:
	W_Hierarchy(M_Editor* editor);
	~W_Hierarchy() {}

	void Draw() override;
};

#endif
