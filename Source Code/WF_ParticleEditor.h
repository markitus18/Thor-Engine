#ifndef __WF_PARTICLE_EDITOR_H__
#define __WF_PARTICLE_EDITOR_H__

#include "WindowFrame.h"

class M_Editor;
class R_ParticleSystem;
class Emitter;
class ParticleModule;

struct ImGuiWindowClass;

class WF_ParticleEditor : public WindowFrame
{
public:
	WF_ParticleEditor(M_Editor* editor, ImGuiWindowClass* frameWindowClass, ImGuiWindowClass* windowClass, int ID);
	~WF_ParticleEditor();

	void SetParticleSystem(R_ParticleSystem* particleSystem);

	void LoadLayout_ForceDefault(Config& file, ImGuiID mainDockID) override;
	static inline const char* GetName() { return "ParticleEditor"; };


private:
	void MenuBar_Custom();
	void MenuBar_Development();

public:
	R_ParticleSystem* particleSystem = nullptr;
	Emitter* selectedEmitter = nullptr;
	ParticleModule* selectedModule = nullptr;

};

#endif //__WF_PARTICLE_EDITOR_H
