#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"

#include "ModuleInput.h"

//Panels
#include "PanelHierarchy.h"
#include "UI_Console.h"

#include "OpenGL.h"

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl_gl3.h"

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleEditor::~ModuleEditor()
{}

bool ModuleEditor::Init()
{
	ImGui_ImplSdlGL3_Init(App->window->window);

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = "imgui.ini";

	//Initializing all panels
	console = new UI_Console();
	panelHierarchy = new PanelHierarchy();

	test_color = ImColor(71, 71, 71);
	return true;
}

update_status ModuleEditor::Update(float dt)
{
	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDrawCursor = true;

	DrawPanels();
	ImGui::Text("Hello, world!");

	//Showing all windows ----------
	if (show_About_window)
		ShowAboutWindow();
	if (show_Demo_window)
		ImGui::ShowTestWindow();
	if (show_Test_window)
		ShowTestWindow();
	if (show_Settings_window)
		ShowSettingsWindow();

	// -----------------------------
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit          ", "Esc"))
			{
				ImGui::EndMenu();
				return UPDATE_STOP;	
			}
			ImGui::EndMenu();

		}
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Console          ", "1", &console->active))
			{
			}
			if (ImGui::MenuItem("Settings         ", "2", &show_Settings_window))
			{
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About Thor Engine   ", NULL, &show_About_window);
			ImGui::Separator();
			if (ImGui::MenuItem("Documentation       "))
			{
				App->RequestBrowser("https://github.com/markitus18/Game-Engine/wiki");
			}
			if (ImGui::MenuItem("Download latest     "))
			{
				App->RequestBrowser("https://github.com/markitus18/Game-Engine/releases");
			}
			if (ImGui::MenuItem("Report a bug        "))
			{
				App->RequestBrowser("https://github.com/markitus18/Game-Engine/issues");
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debugging Tools"))
		{
			ImGui::MenuItem("ImGui Demo", NULL, &show_Demo_window);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (ImGui::Button("Quit", ImVec2(100, 50)))
	{
		return UPDATE_STOP;
	}

	char* button_label = "Open Window";
	if (show_Test_window)
	{
		button_label = "Close Window";
	}

	if (ImGui::Button(button_label, ImVec2(100, 50)))
	{
		show_Test_window ^= 1;
	}

	if (console->active)
	{
		console->Draw("Console", &console->active);
	}

	//Change background color, we use "test_color", controllable variable from UI
	glClearColor(test_color.x, test_color.y, test_color.z, test_color.w);

	ImGui::Render();
	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp()
{
	if (console)
	{
		delete console;
		console = NULL;
	}

	ImGui_ImplSdlGL3_Shutdown();
	return true;
}

void ModuleEditor::Log(const char* input)
{
	if (console != NULL)
		console->AddLog(input);
}

void ModuleEditor::GetEvent(SDL_Event* event)
{
	ImGui_ImplSdlGL3_ProcessEvent(event);
}

void ModuleEditor::DrawPanels()
{
	if (panelHierarchy != NULL)
	{
		panelHierarchy->Draw();
	}
}

void ModuleEditor::ShowAboutWindow()
{
	ImGui::Begin("About Thor Engine", &show_About_window, ImVec2(400, 100), 1.0f);
	ImGui::Text("v0.2-alpha");
	ImGui::Separator();
	ImGui::Text("By Marc Garrigo for educational purposes.");
	ImGui::Text("Thor Engine is licensed under Public Domain, see LICENSE for more information.");
	ImGui::End();
}

void ModuleEditor::ShowTestWindow()
{
	//Setting up window flags
	ImGuiWindowFlags window_flags = 0;
	//Allowing menu bar on the window
	window_flags |= ImGuiWindowFlags_MenuBar;
	ImGui::Begin("Testing windows", &show_Test_window, ImVec2(500, 300), 1.0f, window_flags);
	ImGui::Text("Some random text in here!");

	//Menu Bar
	if (ImGui::BeginMenuBar())
	{
		//Menu Bar -- Menu Button
		if (ImGui::BeginMenu("Menu"))
		{
			ImGui::MenuItem("Disabled button", NULL, false, false);
			ImGui::MenuItem("Menu option 1");
			ImGui::MenuItem("Menu option 2");
			ImGui::MenuItem("Menu option 3");
			ImGui::MenuItem("Menu option 4");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::MenuItem("Nothing in here, sorry!", NULL, false, false);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}


	ImGui::ColorEdit3("Change Bg color", (float*)&test_color);
	ImGui::End();
}

void ModuleEditor::ShowSettingsWindow()
{
	ImGui::Begin("Settings", &show_Settings_window, ImVec2(500, 600), 1.0f);
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
		ImGui::InputText("Project Name", tmp_appName, IM_ARRAYSIZE(tmp_appName));
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(1);
		ImGui::PlotHistogram("FPS", FPS_data, IM_ARRAYSIZE(FPS_data), 0, NULL, 0.0f, 120.0f, ImVec2(0, 80));
		ImGui::PlotHistogram("MS", ms_data, IM_ARRAYSIZE(ms_data), 0, NULL, 0.0f, 40.0f, ImVec2(0, 80));
	}

	if (ImGui::CollapsingHeader("Window"))
	{

	}

	if (ImGui::CollapsingHeader("File System"))
	{

	}

	if (ImGui::CollapsingHeader("Input"))
	{
		ImGui::Text("Mouse position: %i, %i", App->input->GetMouseX(), App->input->GetMouseY());
		ImGui::Text("Mouse motion: %i, %i", App->input->GetMouseXMotion(), App->input->GetMouseYMotion());
		ImGui::Text("Mouse wheel: %i", App->input->GetMouseZ());

	}

	if (ImGui::CollapsingHeader("Hardware"))
	{

	}

	ImGui::End();
}

void ModuleEditor::InitFPSData()
{
	for (int i = 0; i < 100; i++)
	{
		FPS_data[i] = 0;
		ms_data[i] = 0;
	}
}

void ModuleEditor::UpdateFPSData(int fps, int ms)
{
	for (int i = 0; i < 100; i++)
	{
		FPS_data[i] = FPS_data[i + 1];
		ms_data[i] = ms_data[i + 1];
	}
	FPS_data[100 - 1] = fps;
	ms_data[100 - 1] = ms;
}