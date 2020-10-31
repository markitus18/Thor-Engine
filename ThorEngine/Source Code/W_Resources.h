#ifndef __W_RESOURCES_H__
#define __W_RESOURCES_H__

#include "Window.h"

class Resource;

struct ImGuiWindowClass;

class W_Resources : public Window
{
public:
	W_Resources(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID);
	~W_Resources() {}

	void Draw() override;

	static inline const char* GetName() { return "Resources"; };

private:
	void DisplayResourceInfo(Resource* resource);

};

#endif
