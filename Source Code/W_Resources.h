#ifndef __W_RESOURCES_H__
#define __W_RESOURCES_H__

#include "Window.h"

class Resource;

struct ImGuiWindowClass;

class W_Resources : public Window
{
public:
	W_Resources(M_Editor* editor, ImGuiWindowClass* windowClass);
	~W_Resources() {}

	void Draw() override;

	static inline const char* GetName() { return "Resources"; };

private:
	void DisplayResourceInfo(Resource* resource);

};

#endif
