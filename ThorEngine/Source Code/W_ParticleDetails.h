#ifndef __W_PARTICLE_MODULE_H__
#define __W_PARTICLE_MODULE_H__

#include "Window.h"

class M_Editor;
class WF_ParticleEditor;

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

class W_ParticleDetails : public Window
{
public:
	W_ParticleDetails(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID);
	~W_ParticleDetails() {}

	void Draw() override;

	static inline const char* GetName() { return "Details"; };

private:
	void DrawModule(EmitterBase* module);
	void DrawModule(EmitterSpawn* module);
	void DrawModule(EmitterArea* module);
	void DrawModule(ParticlePosition* module);
	void DrawModule(ParticleRotation* module);
	void DrawModule(ParticleSize* module);
	void DrawModule(ParticleColor* module);
	void DrawModule(ParticleLifetime* module);
	void DrawModule(ParticleVelocity* module);
};

#endif // !__W_PARTICLE_MODULE_H__
