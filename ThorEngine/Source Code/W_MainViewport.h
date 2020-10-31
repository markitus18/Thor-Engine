#ifndef __W_SCENE_H__
#define __W_SCENE_H__

#include "WViewport.h"

struct ImGuiWindowClass;

class W_MainViewport : public WViewport
{
public:
	W_MainViewport(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID);
	~W_MainViewport() {}

	virtual void DrawToolbarCustom() override;

	static inline const char* GetName() { return "Scene Viewport"; };
};

#endif
