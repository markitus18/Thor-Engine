#include "W_ParticleToolbar.h"

#include "ImGui/imgui.h"

W_ParticleToolbar::W_ParticleToolbar(M_Editor* editor, ImGuiWindowClass* windowClass, int ID, WF_ParticleEditor* hostWindow) : Window(editor, GetName(), windowClass, ID), hostWindow(hostWindow)
{

}

void W_ParticleToolbar::Draw()
{
	ImGui::SetNextWindowClass(windowClass);
	if (!ImGui::Begin(windowStrID.c_str(), &active)) { ImGui::End(); return; }

	ImGui::End();
}

