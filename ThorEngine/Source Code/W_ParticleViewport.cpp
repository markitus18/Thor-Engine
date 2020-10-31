#include "W_ParticleViewport.h"

#include "ImGui/imgui.h"

W_ParticleViewport::W_ParticleViewport(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID) : Window(parent, GetName(), windowClass, ID)
{

}

void W_ParticleViewport::Draw()
{
	ImGui::SetNextWindowClass(windowClass);
	if (!ImGui::Begin(windowStrID.c_str(), &active)) { ImGui::End(); return; }

	ImGui::End();
}

