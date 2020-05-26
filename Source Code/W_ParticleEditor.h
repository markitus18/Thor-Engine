#ifndef __PARTICLE_EDITOR_H__
#define __PARTICLE_EDITOR_H__

#include "DWindow.h"

class Emitter;
struct ParticleModule;
struct EmitterBase;
struct EmitterSpawn;
struct EmitterArea;
struct ParticlePosition;
struct ParticleRotation;
struct ParticleSize;
struct ParticleColor;
struct ParticleLifetime;
struct ParticleVelocity;

class W_ParticleEditor : public DWindow
{
public:
	W_ParticleEditor(M_Editor* editor);
	~W_ParticleEditor() {}

	void Draw() override;

private:
	std::string GetModuleName(const ParticleModule* module) const;
	void DrawModuleData(ParticleModule* module);

	void DrawModule(EmitterBase* module);
	void DrawModule(EmitterSpawn* module);
	void DrawModule(EmitterArea* module);
	void DrawModule(ParticlePosition* module);
	void DrawModule(ParticleRotation* module);
	void DrawModule(ParticleSize* module);
	void DrawModule(ParticleColor* module);
	void DrawModule(ParticleLifetime* module);
	void DrawModule(ParticleVelocity* module);

private:
	Emitter* selectedEmitter = nullptr;
	ParticleModule* selectedModule = nullptr;
};

#endif // !

