#include "PanelConfiguration.h"

PanelConfiguration::PanelConfiguration()
{

}

PanelConfiguration::~PanelConfiguration()
{

}

void PanelConfiguration::Draw()
{
	if (active)
	{
		ImGui::SetNextWindowPos(ImVec2(position.x, position.y));
		ImGui::SetNextWindowSize(ImVec2(size.x, size.y));

		ImGui::Begin("Configuration", &active, ImVec2(size.x, size.y), 1.0f);
		if (ImGui::BeginMenu("Options"))
		{
			ImGui::MenuItem("Default", NULL, false, false);
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(2);
			ImGui::MenuItem("Save", NULL, false, false);
			ImGui::MenuItem("Load", NULL, false, false);
			ImGui::EndMenu();
		}

		if (ImGui::CollapsingHeader("Application"))
		{
			//ImGui::InputText("Project Name", tmp_appName, IM_ARRAYSIZE(tmp_appName));
			//ImGui::PlotHistogram("FPS", FPS_data, IM_ARRAYSIZE(FPS_data), 0, NULL, 0.0f, 120.0f, ImVec2(0, 80));
			//ImGui::PlotHistogram("MS", ms_data, IM_ARRAYSIZE(ms_data), 0, NULL, 0.0f, 40.0f, ImVec2(0, 80));
		}

		if (ImGui::CollapsingHeader("Window"))
		{

		}

		if (ImGui::CollapsingHeader("File System"))
		{

		}

		if (ImGui::CollapsingHeader("Input"))
		{
			//ImGui::Text("Mouse position: %i, %i", App->input->GetMouseX(), App->input->GetMouseY());
			//ImGui::Text("Mouse motion: %i, %i", App->input->GetMouseXMotion(), App->input->GetMouseYMotion());
			//ImGui::Text("Mouse wheel: %i", App->input->GetMouseZ());
		}

		if (ImGui::CollapsingHeader("Hardware"))
		{

		}

		ImGui::End();
	}
}

void PanelConfiguration::UpdatePosition(int screen_height, int screen_width)
{
	position.x = screen_width * (0.80);
	position.y = screen_height * (0.60);

	size.x = screen_width * (0.20);
	size.y = screen_height - screen_height * (0.60);
}