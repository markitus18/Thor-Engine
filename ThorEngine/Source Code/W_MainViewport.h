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

private:
	ResourceHandle<R_Texture> hTranslateIcon;
	ResourceHandle<R_Texture> hRotateIcon;
	ResourceHandle<R_Texture> hScaleIcon;
	
	ResourceHandle<R_Texture> hLocalGizmoIcon;
	ResourceHandle<R_Texture> hWorldGizmoIcon;

	ResourceHandle<R_Texture> hGridSnapIcon;
	ResourceHandle<R_Texture> hRotationSnapIcon;
	ResourceHandle<R_Texture> hScaleSnapIcon;
};

#endif
