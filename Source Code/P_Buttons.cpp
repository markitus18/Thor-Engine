#include "P_Buttons.h"
#include "Engine.h"
#include "M_Editor.h"

P_Buttons::P_Buttons()
{

}

P_Buttons::~P_Buttons()
{

}

void P_Buttons::Draw(ImGuiWindowFlags default_flags)
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::SetNextWindowPos(position);

	bool open = true;
	if (ImGui::Begin("Wire window", &open, flags))
	{
		ImGui::Checkbox("Shaded", &Engine->moduleEditor->shaded);
		ImGui::Checkbox("Wireframe", &Engine->moduleEditor->wireframe);

		ImGui::End();
	}

	
	ImGui::PopStyleColor();
}

void P_Buttons::UpdatePosition(int screen_width, int screen_height)
{

	position.x = screen_width * (0.22);
	position.y = screen_height * (0.04);

	size.x = screen_width - position.x;
	size.y = screen_height - position.y;
}





