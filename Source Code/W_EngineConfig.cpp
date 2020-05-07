#include "W_EngineConfig.h"

#include "ImGui/imgui.h"

#include "Engine.h"
#include "M_Camera3D.h"
#include "M_Input.h"
#include "M_Editor.h"
#include "M_Renderer3D.h"

#include "W_Scene.h"

#include "Dock.h"
#include "TreeNode.h"

W_EngineConfig::W_EngineConfig(M_Editor* editor) : DWindow(editor, "EngineConfig")
{
	for (int i = 0; i < 100; i++)
	{
		FPS_data[i] = 0;
		ms_data[i] = 0;
	}
}

void W_EngineConfig::Draw()
{
	if (ImGui::BeginMenu("Options"))
	{
		ImGui::MenuItem("Default", NULL, false, false);
		ImGui::MenuItem("Save", NULL, false, false);
		ImGui::MenuItem("Load", NULL, false, false);
		ImGui::EndMenu();
	}
	if (ImGui::CollapsingHeader("Application"))
	{
		char appName[100];
		strcpy_s(appName, 100, Engine->GetTitleName());

		if (ImGui::InputText("Project Name", appName, 100, ImGuiInputTextFlags_EnterReturnsTrue))
			Engine->SetTitleName(appName);

		ImGui::PlotHistogram("FPS", FPS_data, IM_ARRAYSIZE(FPS_data), 0, NULL, 0.0f, 120.0f, ImVec2(0, 80));
		ImGui::PlotHistogram("MS", ms_data, IM_ARRAYSIZE(ms_data), 0, NULL, 0.0f, 40.0f, ImVec2(0, 80));
	}

	if (ImGui::CollapsingHeader("Renderer"))
	{
		bool enabled = Engine->renderer3D->depthEnabled;
		if (ImGui::Checkbox("Depth Buffer", &enabled))
		{
			Engine->renderer3D->SetDepthBufferEnabled(enabled);
		}
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		float3 camera_pos = Engine->camera->GetPosition();
		if (ImGui::DragFloat3("Position", (float*)&camera_pos))
		{
			Engine->camera->SetPosition(camera_pos);
		}

		float3 camera_ref = Engine->camera->GetReference();
		if (ImGui::DragFloat3("Reference", (float*)&camera_ref))
		{
			Engine->camera->Look(camera_ref);
		}
	}

	if (ImGui::CollapsingHeader("Input"))
	{
		ImGui::Text("Mouse position: %i, %i", Engine->input->GetMouseX(), Engine->input->GetMouseY());
		ImGui::Text("Mouse motion: %i, %i", Engine->input->GetMouseXMotion(), Engine->input->GetMouseYMotion());
		ImGui::Text("Mouse wheel: %i", Engine->input->GetMouseZ());
	}

	if (ImGui::CollapsingHeader("Editor: Selection"))
	{
		ImGui::Text("Selected GameObjects");
		ImGui::Separator();
		std::vector<TreeNode*>::iterator it;
		for (it = Engine->moduleEditor->selectedGameObjects.begin(); it != Engine->moduleEditor->selectedGameObjects.end(); ++it)
		{
			ImGui::Text((*it)->GetName());
		}
		ImGui::Separator();
		ImGui::Separator();
		ImGui::Text("To select GameObjects");
		ImGui::Separator();
		for (it = Engine->moduleEditor->toSelectGOs.begin(); it != Engine->moduleEditor->toSelectGOs.end(); ++it)
		{
			ImGui::Text((*it)->GetName());
		}
		ImGui::Separator();
		ImGui::Separator();
		ImGui::Text("To drag GameObjects");
		ImGui::Separator();
		for (it = Engine->moduleEditor->toDragGOs.begin(); it != Engine->moduleEditor->toDragGOs.end(); ++it)
		{
			ImGui::Text((*it)->GetName());
		}
		ImGui::Separator();
		ImGui::Separator();
		ImGui::Text("To unselect GameObjects");
		ImGui::Separator();
		for (it = Engine->moduleEditor->toUnselectGOs.begin(); it != Engine->moduleEditor->toUnselectGOs.end(); ++it)
		{
			ImGui::Text((*it)->GetName());
		}
	}

	if (ImGui::CollapsingHeader("Editor - Scene"))
	{
		ImGui::Text("Image corner: %i, %i", (int)Engine->moduleEditor->w_scene->cornerPos.x, (int)Engine->moduleEditor->w_scene->cornerPos.y);
		ImGui::Text("Image size: %i, %i", (int)Engine->moduleEditor->w_scene->img_size.x, (int)Engine->moduleEditor->w_scene->img_size.y);
	}
}

void W_EngineConfig::OnResize()
{

}

void W_EngineConfig::UpdateFPSData(int fps, int ms)
{
	for (int i = 0; i < 99; i++)
	{
		FPS_data[i] = FPS_data[i + 1];
		ms_data[i] = ms_data[i + 1];
	}
	FPS_data[100 - 1] = fps;
	ms_data[100 - 1] = ms;
}
