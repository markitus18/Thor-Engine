#ifndef __WF_PARTICLE_EDITOR_H__
#define __WF_PARTICLE_EDITOR_H__

#include "WindowFrame.h"

class M_Editor;
class R_ParticleSystem;
class Emitter;
class ParticleModule;
class C_ParticleSystem;

struct ImGuiWindowClass;

class WF_ParticleEditor : public WindowFrame
{
public:
	WF_ParticleEditor(M_Editor* editor, ImGuiWindowClass* frameWindowClass, ImGuiWindowClass* windowClass, int ID);
	~WF_ParticleEditor();

	void SetResource(uint64 resourceID) override;

	void LoadLayout_Default(ImGuiID mainDockID) override;
	static inline const char* GetName() { return "ParticleEditor"; };


private:
	void MenuBar_Custom();
	void MenuBar_Development();

public:
	R_ParticleSystem* particleSystem = nullptr;
	Emitter* selectedEmitter = nullptr;
	ParticleModule* selectedModule = nullptr;

	C_ParticleSystem* sceneParticleSystem = nullptr;
};

#endif //__WF_PARTICLE_EDITOR_H
