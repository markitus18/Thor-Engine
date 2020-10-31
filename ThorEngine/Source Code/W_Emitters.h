#ifndef __W_EMITTERS_H__
#define __W_EMITTERS_H__

#include "Window.h"
#include <string>

class M_Editor;
class WF_ParticleEditor;
struct ParticleModule;

struct ImGuiWindowClass;

class W_Emitters : public Window
{
public:
	W_Emitters(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID);
	~W_Emitters() {}

	void Draw() override;

	static inline const char* GetName() { return "Emitters"; };

private:
	std::string GetModuleName(const ParticleModule* module) const;
};

#endif // !__W_EMITTERS_H__
