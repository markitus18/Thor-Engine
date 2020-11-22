#ifndef __W_PARTICLE_PREVIEW_H__
#define __W_PARTICLE_PREVIEW_H__

#include "WViewport.h"

class WF_ParticleEditor;

class W_ParticleViewport : public WViewport
{
public:
	W_ParticleViewport(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID);
	~W_ParticleViewport() {}

	virtual void DrawToolbarCustom() override;

	static inline const char* GetName() { return "Particles Viewport"; };
};
#endif // !__W_PARTICLE_PREVIEW_H__
