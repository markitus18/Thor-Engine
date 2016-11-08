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

#include "ModuleScene.h"
#include "ModuleFileSystem.h"

#include "OpenGL.h"

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl_gl3.h"

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module("Editor", start_enabled)
{

}

ModuleEditor::~ModuleEditor()
{}

bool ModuleEditor::Init(Config& config)
{
	ImGui_ImplSdlGL3_Init(App->window->window);

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
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

	//Chaning ImGui style
	ImGuiStyle style = ImGui::GetStyle();

	//Window rounding
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	//Window title color
	ImVec4 headerColor = style.Colors[ImGuiCol_TitleBg];
	headerColor.w = 1.0f;
	ImGui::PushStyleColor(ImGuiCol_TitleBg, headerColor);

	headerColor = style.Colors[ImGuiCol_TitleBgActive];
	headerColor.w = 1.0f;
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, headerColor);


	//Button color
	ImVec4 buttonColor(0.37, 0.37, 0.64, 0.6);
	ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);

	buttonColor = ImVec4(0.37, 0.37, 0.64, 0.8);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColor);

	buttonColor = ImVec4(0.347, 0.37, 0.64, 1.0);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);



	return true;
}

update_status ModuleEditor::PreUpdate(float dt)
{
	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	ImGuiIO& io = ImGui::GetIO();

	using_keyboard = io.WantCaptureKeyboard;
	using_mouse = io.WantCaptureMouse;

	return UPDATE_CONTINUE;
}

void ModuleEditor::Draw()
{
	DrawPanels();

	//Showing all windows ----------
	if (show_About_window)
		ShowAboutWindow();
	if (show_Demo_window)
		ImGui::ShowTestWindow(&show_Demo_window);
	if (show_fileName_window)
		ShowFileNameWindow();

	// -----------------------------
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{

			if (ImGui::BeginMenu("Open Scene"))
			{
				//TODO: avoid doing this every frame
				sceneList.clear();
				App->fileSystem->GetAllFilesWithExtension("Assets", "scene", sceneList);
				
				for (uint i = 0; i < sceneList.size(); i++)
				{
					if (ImGui::MenuItem(sceneList[i].c_str()))
					{
						App->LoadScene(sceneList[i].c_str());
					}
				}
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Save Scene"))
			{
				if (App->scene->current_scene == "Untitled")
				{
					OpenFileNameWindow();
				}
				else
				{
					App->SaveScene(App->scene->current_scene.c_str());
				}
			}

			if (ImGui::MenuItem("Save Scene as"))
			{
				OpenFileNameWindow();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit          "))
			{
				ImGui::EndMenu();
				return;
			}
			ImGui::EndMenu();

		}
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Inspector          ", nullptr, &panelInspector->active))
			{
			}
			if (ImGui::MenuItem("Hierarchy          ", nullptr, &panelHierarchy->active))
			{
			}
			if (ImGui::MenuItem("Console          ", nullptr, &panelConsole->active))
			{
			}
			if (ImGui::MenuItem("Configuration         ", nullptr, &panelConfiguration->active))
			{
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About Thor Engine   ", nullptr, &show_About_window);
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
			ImGui::MenuItem("ImGui Demo", nullptr, &show_Demo_window);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
	//----------------------------

	ImGui::Render();
}

bool ModuleEditor::CleanUp()
{
	if (panelConsole)
	{
		delete panelConsole;
		panelConsole = nullptr;
	}

	if (panelHierarchy)
	{
		delete panelHierarchy;
		panelHierarchy = nullptr;
	}

	if (panelInspector)
	{
		delete panelInspector;
		panelInspector = nullptr;
	}

	if (panelConfiguration)
	{
		delete panelConfiguration;
		panelConfiguration = nullptr;
	}

	if (panelButtons)
	{
		delete panelButtons;
		panelButtons = nullptr;
	}

	ImGui_ImplSdlGL3_Shutdown();
	return true;
}

void ModuleEditor::Log(const char* input)
{
	if (panelConsole != nullptr)
		panelConsole->AddLog(input);
}

void ModuleEditor::GetEvent(SDL_Event* event)
{
	ImGui_ImplSdlGL3_ProcessEvent(event);
}

void ModuleEditor::DrawPanels()
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;
	if (panelHierarchy != nullptr)
	{
		panelHierarchy->Draw(windowFlags);
	}

	if (panelConsole != nullptr)
	{
		panelConsole->Draw(windowFlags);
	}

	if (panelInspector != nullptr)
	{
		panelInspector->Draw(windowFlags);
	}

	if (panelConfiguration != nullptr)
	{
		panelConfiguration->Draw(windowFlags);
	}

	if (panelButtons != nullptr)
	{
		panelButtons->Draw(windowFlags);
	}
}

void ModuleEditor::ShowAboutWindow()
{
	ImGui::SetNextWindowSize(ImVec2(600, 100));
	ImGui::Begin("About Thor Engine", &show_About_window, ImVec2(0, 0), 1.0f, ImGuiWindowFlags_NoResize);
	ImGui::Text("v0.4-alpha");
	ImGui::Separator();
	ImGui::Text("By Marc Garrigo for educational purposes.");
	ImGui::Text("Thor Engine is licensed under Public Domain, see LICENSE for more information.");
	ImGui::End();
}

void ModuleEditor::ShowFileNameWindow()
{
	ImGui::SetNextWindowSize(ImVec2(400, 100));
	ImGui::Begin("File Name", &show_fileName_window, ImVec2(0, 0), 1.0f, ImGuiWindowFlags_NoResize);

	if (ImGui::InputText("", fileName, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		App->SaveScene(fileName);
	}

	if (ImGui::Button("Accept"))
	{
		App->SaveScene(fileName);
		show_fileName_window = false;
	}

	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
	{
		show_fileName_window = false;
	}
	ImGui::End();
}

void ModuleEditor::OpenFileNameWindow()
{
	show_fileName_window = true;
	std::string file;
	App->fileSystem->SplitFilePath(App->scene->current_scene.c_str(), nullptr, &file);

	strcpy_s(fileName, 50, file.c_str());
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

bool ModuleEditor::UsingKeyboard() const
{
	return using_keyboard;
}

bool ModuleEditor::UsingMouse() const
{
	return using_mouse;
}

//Timer management -------------------
uint ModuleEditor::AddTimer(const char* text, const char* tag)
{
	return panelConfiguration->AddTimer(text, tag);
}

void ModuleEditor::StartTimer(uint index)
{
	panelConfiguration->StartTimer(index);
}

void ModuleEditor::ReadTimer(uint index)
{
	panelConfiguration->ReadTimer(index);
}

void ModuleEditor::StopTimer(uint index)
{
	panelConfiguration->StopTimer(index);
}

void ModuleEditor::LoadScene(Config& root)
{
	panelHierarchy->selectedGameObjects.clear();
}

void ModuleEditor::ResetScene()
{
	panelHierarchy->selectedGameObjects.clear();
}
//------------------------------------