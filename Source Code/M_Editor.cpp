#include "Application.h"
#include "M_Editor.h"
#include "M_Window.h"

#include "M_Input.h"

//Panels
#include "P_Hierarchy.h"
#include "P_Console.h"
#include "P_Inspector.h"
#include "P_Configuration.h"
#include "P_Buttons.h"

#include "M_Scene.h"
#include "M_FileSystem.h"

#include "GameObject.h"
#include "OpenGL.h"

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl_gl3.h"

#include "Time.h"

M_Editor::M_Editor(bool start_enabled) : Module("Editor", start_enabled)
{

}

M_Editor::~M_Editor()
{}

bool M_Editor::Init(Config& config)
{
	ImGui_ImplSdlGL3_Init(App->window->window);

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	io.MouseDrawCursor = true;


	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	//Initializing all panels
	panelConsole = new P_Console();
	panelHierarchy = new P_Hierarchy();
	panelInspector = new P_Inspector();
	panelConfiguration = new P_Configuration();
	panelButtons = new P_Buttons();
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

update_status M_Editor::PreUpdate(float dt)
{
	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	ImGuiIO& io = ImGui::GetIO();

	using_keyboard = io.WantCaptureKeyboard;
	using_mouse = io.WantCaptureMouse;

	return UPDATE_CONTINUE;
}

void M_Editor::Draw()
{
	DrawPanels();

	//Showing all windows ----------
	if (show_About_window)
		ShowAboutWindow();
	if (show_Demo_window)
		ImGui::ShowTestWindow(&show_Demo_window);
	if (show_fileName_window)
		ShowFileNameWindow();

	ShowPlayWindow();

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

bool M_Editor::CleanUp()
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

void M_Editor::Log(const char* input)
{
	if (panelConsole != nullptr)
		panelConsole->AddLog(input);
}

void M_Editor::GetEvent(SDL_Event* event)
{
	ImGui_ImplSdlGL3_ProcessEvent(event);
}

void M_Editor::DrawPanels()
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

void M_Editor::ShowAboutWindow()
{
	ImGui::SetNextWindowSize(ImVec2(600, 100));
	ImGui::Begin("About Thor Engine", &show_About_window, ImVec2(0, 0), 1.0f, ImGuiWindowFlags_NoResize);
	ImGui::Text("v0.4-alpha");
	ImGui::Separator();
	ImGui::Text("By Marc Garrigo for educational purposes.");
	ImGui::Text("Thor Engine is licensed under Public Domain, see LICENSE for more information.");
	ImGui::End();
}

void M_Editor::ShowPlayWindow()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::SetNextWindowSize(ImVec2(150, 30));
	ImGui::SetNextWindowPos(playWindow);

	bool open = true;
	if (ImGui::Begin("PlayButton", &open, flags))
	{
		std::string name = Time::running ? "Stop" : "Play";
		if (ImGui::Button(name.c_str()))
		{
			Time::running ? App->scene->Stop() : App->scene->Play();
		}
		ImGui::SameLine();
		if (ImGui::Button("Pause"))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button("Frame"))
		{

		}
		ImGui::End();
	}

	ImGui::PopStyleColor();
}

void M_Editor::ShowFileNameWindow()
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

void M_Editor::DeleteSelectedGameObjects()
{
	for (uint i = 0; i < panelHierarchy->selectedGameObjects.size(); i++)
	{
		App->scene->DeleteGameObject(panelHierarchy->selectedGameObjects[i]);
	}
}

void M_Editor::OpenFileNameWindow()
{
	show_fileName_window = true;
	std::string file, extension;
	App->fileSystem->SplitFilePath(App->scene->current_scene.c_str(), nullptr, &file, &extension);
	std::string str = file + extension;
	strcpy_s(fileName, 50, str.c_str());
}

void M_Editor::UpdateFPSData(int fps, int ms)
{
	if (panelConfiguration)
		panelConfiguration->UpdateFPSData(fps, ms);
}

void M_Editor::OnResize(int screen_width, int screen_height)
{
	panelConsole->UpdatePosition(screen_width, screen_height);
	panelHierarchy->UpdatePosition(screen_width, screen_height);
	panelInspector->UpdatePosition(screen_width, screen_height);
	panelConfiguration->UpdatePosition(screen_width, screen_height);
	panelButtons->UpdatePosition(screen_width, screen_height);

	playWindow.x = screen_width / 2 - 100;
	playWindow.y = screen_height / 20;
}

bool M_Editor::UsingKeyboard() const
{
	return using_keyboard;
}

bool M_Editor::UsingMouse() const
{
	return using_mouse;
}

//Timer management -------------------
uint M_Editor::AddTimer(const char* text, const char* tag)
{
	return panelConfiguration->AddTimer(text, tag);
}

void M_Editor::StartTimer(uint index)
{
	panelConfiguration->StartTimer(index);
}

void M_Editor::ReadTimer(uint index)
{
	panelConfiguration->ReadTimer(index);
}

void M_Editor::StopTimer(uint index)
{
	panelConfiguration->StopTimer(index);
}

void M_Editor::SelectGameObject(GameObject* gameObject, bool selectSingle, bool openTree)
{
	if (selectSingle)
		panelHierarchy->SelectSingle(gameObject, openTree);
	else
		panelHierarchy->AddSelect(gameObject, openTree);
}

void M_Editor::LoadScene(Config& root)
{
	panelHierarchy->selectedGameObjects.clear();
}

void M_Editor::ResetScene()
{
	panelHierarchy->selectedGameObjects.clear();
}

void M_Editor::OnRemoveGameObject(GameObject* gameObject)
{
	for (std::vector<GameObject*>::iterator it = panelHierarchy->selectedGameObjects.begin(); it != panelHierarchy->selectedGameObjects.end();)
	{
		if (*it == gameObject)
		{
			it = panelHierarchy->selectedGameObjects.erase(it);
		}
		else
		{
			it++;
		}
	}
}
//------------------------------------