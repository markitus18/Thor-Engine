#include "W_ParticleCurves.h"

#include "ImGui/imgui.h"

W_ParticleCurves::W_ParticleCurves(M_Editor* editor, ImGuiWindowClass* windowClass, int ID, WF_ParticleEditor* hostWindow) : Window(editor, GetName(), windowClass, ID), hostWindow(hostWindow)
{

}

void W_ParticleCurves::Draw()
{
	ImGui::SetNextWindowClass(windowClass);
	if (!ImGui::Begin(windowStrID.c_str(), &active)) { ImGui::End(); return; }

	ImGui::End();
}

