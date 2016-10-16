#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"

#include "ModuleInput.h"

//Panels
#include "PanelHierarchy.h"
#include "PanelConsole.h"
#include "PanelInspector.h"
#include "PanelConfiguration.h"
#include "PanelButtons.h"

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
	io.IniFilename = NULL;
	io.MouseDrawCursor = true;


	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	//Initializing all panels
	panelConsole = new PanelConsole();
	panelHierarchy = new PanelHierarchy();
	panelInspector = new PanelInspector();
	panelConfiguration = new PanelConfiguration();
	panelButtons = new PanelButtons();
	panelConfiguration->Init();

	//Change background color, we use "test_color", controllable variable from UI
	ImVec4 BgColor = ImColor(71, 71, 71);
	glClearColor(BgColor.x, BgColor.y, BgColor.z, BgColor.w);

	//Chaning ImGui style
	ImGuiStyle style = ImGui::GetStyle();

	//Window rounding
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	//Window title color
	ImVec4 headerColor = style.Colors[ImGuiCol_TitleBg];
	headerColor.w = 1.0f;
	ImGui::PushStyleColor(ImGuiCol_TitleBg, headerColor);
	return true;
}

update_status ModuleEditor::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update(float dt)
{
	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	DrawPanels();

	//Showing all windows ----------
	if (show_About_window)
		ShowAboutWindow();
	if (show_Demo_window)
		ImGui::ShowTestWindow(&show_Demo_window);

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
			if (ImGui::MenuItem("Inspector          ", NULL, &panelInspector->active))
			{
			}
			if (ImGui::MenuItem("Hierarchy          ", NULL, &panelHierarchy->active))
			{
			}
			if (ImGui::MenuItem("Console          ", NULL, &panelConsole->active))
			{
			}
			if (ImGui::MenuItem("Configuration         ", NULL, &panelConfiguration->active))
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
	//----------------------------

	ImGui::Render();
	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp()
{
	if (panelConsole)
	{
		delete panelConsole;
		panelConsole = NULL;
	}

	if (panelHierarchy)
	{
		delete panelHierarchy;
		panelHierarchy = NULL;
	}

	if (panelInspector)
	{
		delete panelInspector;
		panelInspector = NULL;
	}

	if (panelConfiguration)
	{
		delete panelConfiguration;
		panelConfiguration = NULL;
	}

	if (panelButtons)
	{
		delete panelButtons;
		panelButtons = NULL;
	}

	ImGui_ImplSdlGL3_Shutdown();
	return true;
}

void ModuleEditor::Log(const char* input)
{
	if (panelConsole != NULL)
		panelConsole->AddLog(input);
}

void ModuleEditor::GetEvent(SDL_Event* event)
{
	ImGui_ImplSdlGL3_ProcessEvent(event);
}

void ModuleEditor::DrawPanels()
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;
	if (panelHierarchy != NULL)
	{
		panelHierarchy->Draw(windowFlags);
	}

	if (panelConsole != NULL)
	{
		panelConsole->Draw(windowFlags);
	}

	if (panelInspector != NULL)
	{
		panelInspector->Draw(windowFlags);
	}

	if (panelConfiguration != NULL)
	{
		panelConfiguration->Draw(windowFlags);
	}

	if (panelButtons != NULL)
	{
		panelButtons->Draw(windowFlags);
	}
}

void ModuleEditor::ShowAboutWindow()
{
	ImGui::SetNextWindowSize(ImVec2(600, 100));
	ImGui::Begin("About Thor Engine", &show_About_window, ImVec2(400, 100), 1.0f, ImGuiWindowFlags_NoResize);
	ImGui::Text("v0.4-alpha");
	ImGui::Separator();
	ImGui::Text("By Marc Garrigo for educational purposes.");
	ImGui::Text("Thor Engine is licensed under Public Domain, see LICENSE for more information.");
	ImGui::End();
}

void ModuleEditor::UpdateFPSData(int fps, int ms)
{
	if (panelConfiguration)
		panelConfiguration->UpdateFPSData(fps, ms);
}

void ModuleEditor::OnResize(int screen_width, int screen_height)
{
	panelConsole->UpdatePosition(screen_width, screen_height);
	panelHierarchy->UpdatePosition(screen_width, screen_height);
	panelInspector->UpdatePosition(screen_width, screen_height);
	panelConfiguration->UpdatePosition(screen_width, screen_height);
	panelButtons->UpdatePosition(screen_width, screen_height);
}