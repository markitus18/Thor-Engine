#include "P_Configuration.h"
#include "Application.h"
#include "M_Input.h"
#include "M_Camera3D.h"

P_Configuration::P_Configuration()
{
}

P_Configuration::~P_Configuration()
{

}

void P_Configuration::Draw(ImGuiWindowFlags flags)
{
	if (active)
	{
		ImGui::SetNextWindowPos(position);
		ImGui::SetNextWindowSize(size);

		if (!ImGui::Begin("Configuration", &active, flags))
		{
			ImGui::End();
			return;
		}
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
			strcpy_s(appName, 100, App->GetTitleName());

			if (ImGui::InputText("Project Name", appName, 100, ImGuiInputTextFlags_EnterReturnsTrue))
				App->SetTitleName(appName);

			ImGui::PlotHistogram("FPS", FPS_data, IM_ARRAYSIZE(FPS_data), 0, NULL, 0.0f, 120.0f, ImVec2(0, 80));
			ImGui::PlotHistogram("MS", ms_data, IM_ARRAYSIZE(ms_data), 0, NULL, 0.0f, 40.0f, ImVec2(0, 80));
		}

		if (ImGui::CollapsingHeader("Camera"))
		{
			float3 camera_pos = App->camera->GetPosition();
			if (ImGui::DragFloat3("Position", (float*)&camera_pos))
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

			for (uint i = 0; i < timers.size(); i++)
			{
				if (timers[i].tag == timerButtons.GetActiveTag())
				{
					ImGui::Text("%s %f ms", timers[i].text.c_str(), timers[i].timer_read);

				}
			}
		}

		ImGui::End();
	}
}

void P_Configuration::UpdatePosition(int screen_width, int screen_height)
{
	position.x = screen_width * (0.80);
	position.y = screen_height * (0.60);

	size.x = screen_width - position.x;
	size.y = screen_height - position.y;
}

void P_Configuration::Init()
{
	for (int i = 0; i < 100; i++)
	{
		FPS_data[i] = 0;
		ms_data[i] = 0;
	}
}

void P_Configuration::UpdateFPSData(int fps, int ms)
{
	for (int i = 0; i < 99; i++)
	{
		FPS_data[i] = FPS_data[i + 1];
		ms_data[i] = ms_data[i + 1];
	}
	FPS_data[100 - 1] = fps;
	ms_data[100 - 1] = ms;
}

uint P_Configuration::AddTimer(const char* text, const char* tag)
{
	timers.push_back(ConfigTimer(text));

	timers[timers.size() - 1].text_end = timers[timers.size() - 1].text.size() - 1;
	timers[timers.size() - 1].text.append(1, ':');
	uint size_to_append = text_spacing - timers[timers.size() - 1].text.size();
	timers[timers.size() - 1].text.append(size_to_append, ' ');

	//Tag management
	timers[timers.size() - 1].tag = tag;

	bool tagFound = false;
	for (uint i = 0; i < tags.size(); i++)
	{
		if (tags[i] == tag)
			tagFound = true;
	}
	if (!tagFound)
	{
		tags.push_back(tag);
		timerButtons.AddButton(tag);
	}
	//-------------

	return timers.size() - 1;
}

void P_Configuration::StartTimer(uint index)
{
	if (index < timers.size())
	{
		timers[index].timer.Start();
	}
}

void P_Configuration::ReadTimer(uint index)
{
	if (index < timers.size())
	{
		timers[index].timer_read = timers[index].timer.ReadMs();
	}
}

//TODO: add timer stop function
void P_Configuration::StopTimer(uint index)
{
	if (index < timers.size())
	{
	//	timers[index].time.
	}
}