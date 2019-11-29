#ifndef __PARTICLE_EDITOR_H__
#define __PARTICLE_EDITOR_H__

#include "DWindow.h"

class Emitter;
class ParticleModule;

class W_ParticleEditor : public DWindow
{
public:
	W_ParticleEditor(M_Editor* editor);
	~W_ParticleEditor() {}

	void Draw();
	void OnResize();

	std::string GetModuleName(const ParticleModule* module) const;

private:
	Emitter* selectedEmitter = nullptr;
	ParticleModule* selectedModule = nullptr;
};

#endif // !

