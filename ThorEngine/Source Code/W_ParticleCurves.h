#ifndef __W_PARTICLE_CURVES_H__
#define __W_PARTICLE_CURVERS_H__

#include "Window.h"

class M_Editor;
class WF_ParticleEditor;

class W_ParticleCurves : public Window
{
public:
	W_ParticleCurves(M_Editor* editor, ImGuiWindowClass* windowClass, int ID, WF_ParticleEditor* hostWindow);
	~W_ParticleCurves() {}

	void Draw() override;

	static inline const char* GetName() { return "Curve Editor"; };

private:
	WF_ParticleEditor* hostWindow = nullptr;
};
#endif // !__W_PARTICLE_CURVES_H__

