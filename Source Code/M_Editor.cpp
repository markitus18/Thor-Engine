#include "Application.h"
#include "M_Editor.h"
#include "M_Window.h"

#include "M_Input.h"

//Panels
#include "P_Buttons.h"

//Windows
#include "Dock.h"
#include "W_Scene.h"
#include "W_Hierarchy.h""
#include "W_Inspector.h"
#include "W_Explorer.h"
#include "W_Console.h"
#include "W_Resources.h"
#include "W_EngineConfig.h"

#include "M_Scene.h"
#include "M_FileSystem.h"
#include "M_Resources.h"
#include "M_Camera3D.h"

#include "GameObject.h"
#include "Resource.h"

#include "OpenGL.h"

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl_gl3.h"

#include "Time.h"


#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )

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
	buttons = new P_Buttons();

	//Creating dock base windows
	Dock* baseDock = new Dock("BaseWindowDock", App->window->windowSize);
	docks.push_back(baseDock);
	baseDock->position = Vec2(0.0f, 19.0f);
	baseDock->Split(VERTICAL, 0.8f);
	baseDock->GetDockChildren()[0]->Split(HORIZONTAL, 0.7f);
	baseDock->GetDockChildren()[0]->GetDockChildren()[0]->Split(VERTICAL, 0.2f);
	baseDock->GetDockChildren()[1]->Split(HORIZONTAL, 0.6f);

	W_Hierarchy* hierarchyWindow = new W_Hierarchy(this);
	baseDock->GetDockChildren()[0]->GetDockChildren()[0]->GetDockChildren()[0]->AddChildData(hierarchyWindow);

	W_Scene* sceneWindow = new W_Scene(this);
	baseDock->GetDockChildren()[0]->GetDockChildren()[0]->GetDockChildren()[1]->AddChildData(sceneWindow);

	W_Inspector* inspector = new W_Inspector(this);
	baseDock->GetDockChildren()[1]->GetDockChildren()[0]->AddChildData(inspector);

	w_explorer = new W_Explorer(this);
	baseDock->GetDockChildren()[0]->GetDockChildren()[1]->AddChildData(w_explorer);

	w_console = new W_Console(this);
	baseDock->GetDockChildren()[0]->GetDockChildren()[1]->AddChildData(w_console);

	W_Resources* resources = new W_Resources(this);
	baseDock->GetDockChildren()[1]->GetDockChildren()[1]->AddChildData(resources);

	w_econfig = new W_EngineConfig(this);
	baseDock->GetDockChildren()[1]->GetDockChildren()[1]->AddChildData(w_econfig);
	
	return true;
}

bool M_Editor::Start()
{
	//TODO: load in another module (use icons as a resource)
	char* buffer = nullptr;
	uint size = App->fileSystem->Load("ProjectSettings/Icons/FolderIcon.png", &buffer);
	if (size > 0)
	{
		if (ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, size))
		{
			w_explorer->folderBuffer = ilutGLBindTexImage();
		}
		RELEASE_ARRAY(buffer);
	}

	size = App->fileSystem->Load("ProjectSettings/Icons/FileIcon.png", &buffer);
	if (size > 0)
	{
		if (ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, size))
		{
			w_explorer->fileBuffer = ilutGLBindTexImage();
		}
		RELEASE_ARRAY(buffer);
	}

	size = App->fileSystem->Load("ProjectSettings/Icons/SelectedIcon.png", &buffer);
	if (size > 0)
	{
		if (ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, size))
		{
			w_explorer->selectedBuffer = ilutGLBindTexImage();
		}
		RELEASE_ARRAY(buffer);
	}
	glBindTexture(GL_TEXTURE_2D, 0); //Soo... this needs to be done in order to reset the texture buffer
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
	//DrawPanels();
	docks[0]->Draw(); //TOOD: Do a proper data access

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		DeleteSelected();
	}

	if (dragging == true && (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP || App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_IDLE))
	{
		toDragGOs.clear();
		FinishDrag(true, false);
		dragging = false;
	}

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
			if (ImGui::MenuItem("New Scene"))
			{
				App->scene->CreateDefaultScene();
			}

			if (ImGui::BeginMenu("Open Scene"))
			{
				//TODO: avoid doing this every frame
				sceneList.clear();
				App->fileSystem->GetAllFilesWithExtension("", "scene", sceneList);
				
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
				ImGui::EndMainMenuBar();
				return;
			}
			ImGui::EndMenu();

		}

		if (ImGui::BeginMenu("Assets"))
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Camera"))
				{
					App->scene->CreateCamera();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Load Imported Scene"))
			{
				//TODO: avoid doing this every frame
				PathNode assets = App->moduleResources->CollectImportedScenes();
				DisplayScenesWindows(assets);

				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::MenuItem("Create Empty"))
			{
				std::string name(App->scene->GetNewGameObjectName("GameObject"));
				GameObject* newGameObject = App->scene->CreateGameObject(name.c_str());
				SelectSingle(newGameObject);
			}

			if (ImGui::MenuItem("Create Empty Child"))
			{
				GameObject* parent = (GameObject*)(selectedGameObjects.size() > 0 ? selectedGameObjects[0] : nullptr);
				std::string name(App->scene->GetNewGameObjectName("GameObject", parent));
				GameObject* newGameObject = App->scene->CreateGameObject(name.c_str(), parent);
				SelectSingle(newGameObject);
			}

			if (ImGui::MenuItem("Create Empty x10"))
			{
				for (uint i = 0; i < 10; i++)
				{
					std::string name(App->scene->GetNewGameObjectName("GameObject"));
					GameObject* newGameObject = App->scene->CreateGameObject(name.c_str());
					SelectSingle(newGameObject);
				}
			}
			if (ImGui::BeginMenu("3D Object"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					std::string name(App->scene->GetNewGameObjectName("Cube"));
					GameObject* newGameObject = App->scene->CreateGameObject(name.c_str());
					SelectSingle(newGameObject);

				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			//TODO: fins a way to activate / deactivate windows
			/*
			if (ImGui::MenuItem("Inspector          ", nullptr, &inspector->active))
			{
			}
			if (ImGui::MenuItem("Hierarchy          ", nullptr, &hierarchy->active))
			{
			}
			if (ImGui::MenuItem("Console          ", nullptr, &console->active))
			{
				if (console->active == true)
				{
					w_explorer->explorerActive = false;
				}
			}
			if (ImGui::MenuItem("Configuration         ", nullptr, &configuration->active))
			{

			}
			if (ImGui::MenuItem("Explorer         ", nullptr, &explorer->active))
			{
				if (explorer->active == true)
				{
					console->active = false;
					resources->active = false;
					explorer->explorerActive = true;
				}
			}
			if (ImGui::MenuItem("Resources         ", nullptr, &resources->active))
			{
				if (resources->active == true)
				{
					explorer->active = false;
				}
			}
			*/
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
			if (ImGui::BeginMenu("Display"))
			{
				ImGui::MenuItem("Quadtree", nullptr, &App->scene->drawQuadtree);
				ImGui::MenuItem("Ray picking", nullptr, &App->camera->drawRay);
				ImGui::MenuItem("GameObjects box", nullptr, &App->scene->drawBounds);
				ImGui::MenuItem("GameObjects box (selected)", nullptr, &App->scene->drawBoundsSelected);
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
	//----------------------------

	
	ImGui::Render();
}

bool M_Editor::CleanUp()
{
	//TODO: remove docking memory
	RELEASE(buttons);

	ImGui_ImplSdlGL3_Shutdown();
	return true;
}

void M_Editor::Log(const char* input)
{
	if (w_console != nullptr)
		w_console->AddLog(input);
}

void M_Editor::GetEvent(SDL_Event* event)
{
	ImGui_ImplSdlGL3_ProcessEvent(event);
}

void M_Editor::DrawPanels()
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	if (buttons != nullptr)
	{
		buttons->Draw(windowFlags);
	}
}

void M_Editor::DisplayScenesWindows(const PathNode& folder)
{
	if (folder.file == true)
	{
		if (ImGui::MenuItem(folder.localPath.c_str()))
		{
			App->moduleResources->LoadPrefab(folder.path.c_str());
		}
	}

	//If node folder has something inside
	else if (folder.leaf == false)
	{
		if (ImGui::BeginMenu(folder.localPath.c_str()))
		{
			for (uint i = 0; i < folder.children.size(); i++)
			{
				DisplayScenesWindows(folder.children[i]);
			}
			ImGui::EndMenu();
		}
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
		std::string name2 = Time::paused ? "Resmue" : "Pause";
		if (ImGui::Button(name2.c_str()))
		{
			Time::paused ? Time::Resume() : Time::Pause();
		}
		//ImGui::SameLine();
		//if (ImGui::Button("Frame"))
		//{

		//}
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

void M_Editor::OpenFileNameWindow()
{
	show_fileName_window = true;
	std::string file, extension;
	App->fileSystem->SplitFilePath(App->scene->current_scene.c_str(), nullptr, &file, &extension);
	std::string str = file;
	if (extension != "")
		file.append("." + extension);
	strcpy_s(fileName, 50, str.c_str());
}

void M_Editor::UpdateFPSData(int fps, int ms)
{
	if (w_econfig)
		w_econfig->UpdateFPSData(fps, ms);
}

void M_Editor::OnResize(int screen_width, int screen_height)
{
	buttons->UpdatePosition(screen_width, screen_height);

	playWindow.x = screen_width / 2 - 90;
	playWindow.y = screen_height / 20;
	
	docks[0]->SetSize(Vec2(screen_width, screen_height));
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
	return 0;
}

void M_Editor::StartTimer(uint index)
{

}

void M_Editor::ReadTimer(uint index)
{
}

void M_Editor::StopTimer(uint index)
{

}


//Selection----------------------------
void M_Editor::SelectSingle(TreeNode* node, bool openTree)
{
	UnselectAll();

	if (node)
	{
		node->Select();
		lastSelected = node;
		selectedGameObjects.push_back(node);

		if (openTree)
		{
			//Opening tree hierarchy node
			TreeNode* it = node->GetParentNode();
			while (it != nullptr)
			{
				it->beenSelected = true;
				it = it->GetParentNode();
			}
		}
	}
}

void M_Editor::AddSelect(TreeNode* node, bool openTree)
{
	UnselectResources();
	if (node && node->IsSelected() == false)
	{
		node->Select();
		selectedGameObjects.push_back(node);

		if (openTree)
		{
			//Opening tree hierarchy node
			TreeNode* it = node->GetParentNode();
			while (it != nullptr)
			{
				it->beenSelected = true;
				it = it->GetParentNode();
			}
		}
	}
}

void M_Editor::AddToSelect(TreeNode* node)
{
	toSelectGOs.push_back(node);
}

void M_Editor::UnselectSingle(TreeNode* node)
{
	node->Unselect();
	std::vector<TreeNode*>::iterator it = selectedGameObjects.begin();
	while (it != selectedGameObjects.end())
	{
		if ((*it) == node)
		{
			selectedGameObjects.erase(it);
			break;
		}
		it++;
	}
}

void M_Editor::UnselectAll()
{
	UnselectResources();
	UnselectGameObjects();
}

void M_Editor::UnselectGameObjects()
{
	for (uint i = 0; i < selectedGameObjects.size(); i++)
	{
		selectedGameObjects[i]->Unselect();
	}
	selectedGameObjects.clear();
}

void M_Editor::UnselectResources()
{
	selectedResources.clear();
}

void M_Editor::DeleteSelected()
{
	//Warning: iterator is not moved because GameObject will be erased from vector on "OnRemove" call
	for (uint i = 0; i < selectedGameObjects.size(); )
	{
		selectedGameObjects[i]->Unselect();
		if (selectedGameObjects[i]->GetType() == GAMEOBJECT)
		{
			App->scene->DeleteGameObject((GameObject*)selectedGameObjects[i]);
		}
		else
		{
			//TODO: delete resource
		}
	}
	selectedGameObjects.clear();

	//TODO: Delete resources
	selectedResources.clear();
}
//Endof Selection -----------------------

void M_Editor::FinishDrag(bool drag, bool selectDrag)
{
	std::vector<TreeNode*>::const_iterator it;
	for (it = toUnselectGOs.begin(); it != toUnselectGOs.end(); it++)
	{
		UnselectSingle(*it);
	}
	if (drag == true)
	{
		if (selectDrag == true)
		{
			for (it = toDragGOs.begin(); it != toDragGOs.end(); it++)
			{
				AddSelect(*it);
			}
		}
	}
	else
	{
		for (it = toSelectGOs.begin(); it != toSelectGOs.end(); it++)
		{
			AddSelect(*it);
		}
	}

	toDragGOs.clear();
	toSelectGOs.clear();
	toUnselectGOs.clear();

}

void M_Editor::LoadScene(Config& root, bool tmp)
{
	selectedGameObjects.clear();
	toSelectGOs.clear();
	toUnselectGOs.clear();
	toDragGOs.clear();
	dragging = false;
}

void M_Editor::ResetScene()
{
	selectedGameObjects.clear();
	toSelectGOs.clear();
	toUnselectGOs.clear();
	toDragGOs.clear();
	dragging = false;
}

void M_Editor::OnRemoveGameObject(GameObject* gameObject)
{
	for (std::vector<TreeNode*>::iterator it = selectedGameObjects.begin(); it != selectedGameObjects.end();)
	{
		if (*it == gameObject)
		{
			it = selectedGameObjects.erase(it);
			break;
		}
		else
		{
			it++;
		}
	}
	if (lastSelected == gameObject) lastSelected = nullptr;
}
//------------------------------------