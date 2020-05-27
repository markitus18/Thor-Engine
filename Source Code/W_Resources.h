#ifndef __W_RESOURCES_H__
#define __W_RESOURCES_H__

#include "Window.h"

class Resource;

class W_Resources : public Window
{
public:
	W_Resources(M_Editor* editor);
	~W_Resources() {}

	void Draw() override;

	void DisplayResourceInfo(Resource* resource);

};

#endif
