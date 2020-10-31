#include "W_ParticleToolbar.h"

#include "ImGui/imgui.h"

W_ParticleToolbar::W_ParticleToolbar(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID) : Window(parent, GetName(), windowClass, ID)
{

}

void W_ParticleToolbar::Draw()
{
	ImGui::SetNextWindowClass(windowClass);
	if (!ImGui::Begin(windowStrID.c_str(), &active)) { ImGui::End(); return; }

	ImGui::End();
}

