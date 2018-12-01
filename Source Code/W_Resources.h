#ifndef __W_RESOURCES_H__
#define __W_RESOURCES_H__

#include "DWindow.h"

class Resource;

class W_Resources : public DWindow
{
public:
	W_Resources(M_Editor* editor);
	~W_Resources() {}

	void Draw();
	void OnResize();

	void DisplayResourceInfo(Resource* resource);

};

#endif
