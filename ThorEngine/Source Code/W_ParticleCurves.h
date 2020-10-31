#ifndef __W_PARTICLE_CURVES_H__
#define __W_PARTICLE_CURVERS_H__

#include "Window.h"

class M_Editor;
class WF_ParticleEditor;

class W_ParticleCurves : public Window
{
public:
	W_ParticleCurves(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID);
	~W_ParticleCurves() {}

	void Draw() override;

	static inline const char* GetName() { return "Curve Editor"; };
};
#endif // !__W_PARTICLE_CURVES_H__

