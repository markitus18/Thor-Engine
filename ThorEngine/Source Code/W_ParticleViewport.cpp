#include "W_ParticleViewport.h"

#include "ImGui/imgui.h"

W_ParticleViewport::W_ParticleViewport(M_Editor* editor, ImGuiWindowClass* windowClass, int ID, WF_ParticleEditor* hostWindow) : Window(editor, GetName(), windowClass, ID), hostWindow(hostWindow)
{

}

void W_ParticleViewport::Draw()
{
	ImGui::SetNextWindowClass(windowClass);
	if (!ImGui::Begin(windowStrID.c_str(), &active)) { ImGui::End(); return; }

	ImGui::End();
}

