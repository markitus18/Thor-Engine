#include "PanelButtons.h"
#include "Application.h"
#include "ModuleEditor.h"

PanelButtons::PanelButtons()
{

}

PanelButtons::~PanelButtons()
{

}

void PanelButtons::Draw(ImGuiWindowFlags default_flags)
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::SetNextWindowPos(ImVec2(position.x, position.y));

	bool open = true;
	if (ImGui::Begin("Wire window", &open, flags))
	{
		ImGui::Checkbox("Shaded", &App->moduleEditor->shaded);
		ImGui::Checkbox("Wireframe", &App->moduleEditor->wireframe);

		ImGui::End();
	}

	ImGui::PopStyleColor();
}

void PanelButtons::UpdatePosition(int screen_width, int screen_height)
{

	position.x = screen_width * (0.22);
	position.y = screen_height * (0.04);

	size.x = screen_width - position.x;
	size.y = screen_height - position.y;
}





