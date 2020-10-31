#ifndef __W_PARTICLE_PREVIEW_H__
#define __W_PARTICLE_PREVIEW_H__

#include "Window.h"

class WF_ParticleEditor;

//TODO: This will inherit from W_Preview as the structure gets ready
class W_ParticleViewport : public Window
{
public:
	W_ParticleViewport(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID);
	~W_ParticleViewport() {}

	void Draw() override;

	static inline const char* GetName() { return "Viewport"; };
};
#endif // !__W_PARTICLE_PREVIEW_H__
