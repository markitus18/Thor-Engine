#include "W_ParticleCurves.h"

#include "ImGui/imgui.h"

W_ParticleCurves::W_ParticleCurves(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID) : Window(parent, GetName(), windowClass, ID)
{

}

void W_ParticleCurves::Draw()
{
	ImGui::SetNextWindowClass(windowClass);
	if (!ImGui::Begin(windowStrID.c_str(), &active)) { ImGui::End(); return; }

	ImGui::End();
}

