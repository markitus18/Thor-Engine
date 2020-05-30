#ifndef __W_PARTICLE_TOOLBAR_H__
#define __W_PARTICLE_TOOLBAR_H__

#include "Window.h"

class WF_ParticleEditor;

class W_ParticleToolbar : public Window
{
public:
	W_ParticleToolbar(M_Editor* editor, ImGuiWindowClass* windowClass, int ID, WF_ParticleEditor* hostWindow);
	~W_ParticleToolbar() {}

	void Draw() override;

	static inline const char* GetName() { return "Toolbar"; };

private:
	WF_ParticleEditor* hostWindow = nullptr;

};
#endif // !__W_PARTICLE_PREVIEW_H__

