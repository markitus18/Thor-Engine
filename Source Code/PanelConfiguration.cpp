#include "PanelConfiguration.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera3D.h"

PanelConfiguration::PanelConfiguration()
{
	timerButtons.AddButton("Culling");
	timerButtons.AddButton("Render");
	timerButtons.AddButton("Test2");
	timerButtons.AddButton("Test3");
	timerButtons.AddButton("Test4");
	timerButtons.AddButton("Test5");
	timerButtons.AddButton("Test6");
	timerButtons.AddButton("Test7");
	timerButtons.AddButton("Test8");
	timerButtons.AddButton("Test9");
}

PanelConfiguration::~PanelConfiguration()
{

}

void PanelConfiguration::Draw(ImGuiWindowFlags flags)
{
	if (active)
	{
		ImGui::SetNextWindowPos(ImVec2(position.x, position.y));
		ImGui::SetNextWindowSize(ImVec2(size.x, size.y));

		if (!ImGui::Begin("Configuration", &active, ImVec2(size.x, size.y), 1.0f, flags))
		{
			ImGui::End();
			return;
		}
		if (ImGui::BeginMenu("Options"))
		{
			ImGui::MenuItem("Default", NULL, false, false);
			//if (ImGui::IsItemHovered())
			//	ImGui::SetMouseCursor(2);
			ImGui::MenuItem("Save", NULL, false, false);
			ImGui::MenuItem("Load", NULL, false, false);
			ImGui::EndMenu();
		}
		if (ImGui::CollapsingHeader("Application"))
		{
			//TODO: This will be moved into module editor and use listeners structure
			char appName[100];
			strcpy_s(appName, 100, App->GetWindowTitle());

			if (ImGui::InputText("Project Name", appName, 100, ImGuiInputTextFlags_EnterReturnsTrue))
				App->SetWindowTitle(appName);
			ImGui::PlotHistogram("FPS", FPS_data, IM_ARRAYSIZE(FPS_data), 0, NULL, 0.0f, 120.0f, ImVec2(0, 80));
			ImGui::PlotHistogram("MS", ms_data, IM_ARRAYSIZE(ms_data), 0, NULL, 0.0f, 40.0f, ImVec2(0, 80));
		}

		if (ImGui::CollapsingHeader("Camera"))
		{
			float3 camera_pos = App->camera->GetPosition();
			if (ImGui::DragFloat("Position", (float*)&camera_pos))
			{
				App->camera->SetPosition(camera_pos);
			}

			float3 camera_ref = App->camera->GetReference();
			if (ImGui::DragFloat3("Reference", (float*)&camera_ref))
			{
				App->camera->Look(camera_ref);
			}
		}

		//if (ImGui::CollapsingHeader("File System"))
		//{

		//}

		if (ImGui::CollapsingHeader("Input"))
		{
			ImGui::Text("Mouse position: %i, %i", App->input->GetMouseX(), App->input->GetMouseY());
			ImGui::Text("Mouse motion: %i, %i", App->input->GetMouseXMotion(), App->input->GetMouseYMotion());
			ImGui::Text("Mouse wheel: %i", App->input->GetMouseZ());
		}


		if (ImGui::CollapsingHeader("Performance Timers"))
		{
			timerButtons.Draw();

			switch (timerButtons.active_button)
			{
				case 0:
				{
					for (uint i = 0; i < timers.size(); i++)
					{
						ImGui::Text("%s %f ms", timers[i].text.c_str(), timers[i].timer_read);
					}
					break;
				}
			}
		}

		ImGui::End();
	}
}

void PanelConfiguration::UpdatePosition(int screen_width, int screen_height)
{
	position.x = screen_width * (0.80);
	position.y = screen_height * (0.60);

	size.x = screen_width - position.x;
	size.y = screen_height - position.y;
}

void PanelConfiguration::Init()
{
	for (int i = 0; i < 100; i++)
	{
		FPS_data[i] = 0;
		ms_data[i] = 0;
	}
}

void PanelConfiguration::UpdateFPSData(int fps, int ms)
{
	for (int i = 0; i < 99; i++)
	{
		FPS_data[i] = FPS_data[i + 1];
		ms_data[i] = ms_data[i + 1];
	}
	FPS_data[100 - 1] = fps;
	ms_data[100 - 1] = ms;
}

uint PanelConfiguration::AddTimer(const char* text)
{
	timers.push_back(ConfigTimer(text));

	timers[timers.size() - 1].text_end = timers[timers.size() - 1].text.size() - 1;
	timers[timers.size() - 1].text.append(1, ':');
	uint size_to_append = text_spacing - timers[timers.size() - 1].text.size();
	timers[timers.size() - 1].text.append(size_to_append, ' ');

	return timers.size() - 1;
}

void PanelConfiguration::StartTimer(uint index)
{
	if (index < timers.size())
	{
		timers[index].timer.Start();
	}
}

void PanelConfiguration::ReadTimer(uint index)
{
	if (index < timers.size())
	{
		timers[index].timer_read = timers[index].timer.ReadMs();
	}
}

//TODO: add timer stop function
void PanelConfiguration::StopTimer(uint index)
{
	if (index < timers.size())
	{
	//	timers[index].time.
	}
}