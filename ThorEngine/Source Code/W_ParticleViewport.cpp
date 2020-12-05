#include "W_ParticleViewport.h"

#include "ImGui/imgui.h"

W_ParticleViewport::W_ParticleViewport(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID) : WViewport(parent, GetName(), windowClass, ID)
{
	statsMask = EViewportStats::FPS;
	statsDisplayed = statsMask; //Temporal, to display all stats at the start
	ImGuizmo::Enable(true);
}

void W_ParticleViewport::DrawToolbarCustom()
{

}


