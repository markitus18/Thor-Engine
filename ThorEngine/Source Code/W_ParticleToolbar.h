#ifndef __W_PARTICLE_TOOLBAR_H__
#define __W_PARTICLE_TOOLBAR_H__

#include "Window.h"

class WF_ParticleEditor;

class W_ParticleToolbar : public Window
{
public:
	W_ParticleToolbar(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID);
	~W_ParticleToolbar() {}

	void Draw() override;

	static inline const char* GetName() { return "Toolbar"; };
};
#endif // !__W_PARTICLE_PREVIEW_H__

