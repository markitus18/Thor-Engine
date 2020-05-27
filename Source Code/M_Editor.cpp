#include "Engine.h"
#include "Config.h"
#include "M_Editor.h"
#include "M_Window.h"

#include "M_Input.h"

//Windows
#include "WF_MainWindow.h"
#include "W_Console.h"
#include "W_EngineConfig.h"

#include "M_Scene.h"
#include "M_FileSystem.h"
#include "M_Resources.h"
#include "M_Renderer3D.h"

#include "GameObject.h"
#include "Resource.h"

#include "OpenGL.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "Time.h"

M_Editor::M_Editor(bool start_enabled) : Module("Editor", start_enabled)
{

}

M_Editor::~M_Editor()
{}

bool M_Editor::Init(Config& config)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);

	ImGui_ImplSDL2_InitForOpenGL(Engine->window->window, Engine->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	io.IniFilename = nullptr;
	io.MouseDrawCursor = false;
	
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	//TODO: not sure if this should be done here too, but it's kinda valid
	LoadConfig(config);

	return true;
}

bool M_Editor::Start()
{
	//TODO: Some mid-way functions can be removed
	CreateWindows();
	
	return true;
}

update_status M_Editor::PreUpdate(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(Engine->window->window);
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();

	using_keyboard = io.WantCaptureKeyboard;
	using_mouse = io.WantCaptureMouse;
	
	return UPDATE_CONTINUE;
}

void M_Editor::CreateWindows()
{
	windowFrames.push_back(new WF_MainWindow(this));

	LoadLayout();
}

void M_Editor::LoadLayout()
{
	//We generate a fake ImGui frame in order to load the layout correctly and set up all docking data
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(Engine->window->window);
	ImGui::NewFrame();

	windowFrames[0]->LoadLayout(Config());

	ImGui::EndFrame();
	ImGui::UpdatePlatformWindows();
}

void M_Editor::Draw()
{
	if (Engine->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		DeleteSelected();
	}

	if (dragging == true && (Engine->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP || Engine->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_IDLE))
	{
		toDragGOs.clear();
		FinishDrag(true, false);
		dragging = false;
	}

	windowFrames[0]->Draw();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
}

bool M_Editor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}

void M_Editor::Log(const char* input)
{
	//TODO: Remove dirty access as the code gets cleaner.
	if (windowFrames.size() > 0 && windowFrames[0]->windows[4] != nullptr)
		((W_Console*)windowFrames[0]->windows[4])->AddLog(input);
}

void M_Editor::GetEvent(SDL_Event* event)
{
	ImGui_ImplSDL2_ProcessEvent(event);
}

void M_Editor::ShowPlayWindow()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::SetNextWindowSize(ImVec2(150, 30));

	bool open = true;
	if (ImGui::Begin("PlayButton", &open, flags))
	{
		std::string name = Time::running ? "Stop" : "Play";
		if (ImGui::Button(name.c_str()))
		{
			Time::running ? Engine->scene->Stop() : Engine->scene->Play();
		}
		ImGui::SameLine();
		std::string name2 = Time::paused ? "Resmue" : "Pause";
		if (ImGui::Button(name2.c_str()))
		{
			Time::paused ? Time::Resume() : Time::Pause();
		}

		ImGui::End();
	}

	ImGui::PopStyleColor();
}

void M_Editor::ShowFileNameWindow()
{
	ImGui::SetNextWindowSize(ImVec2(400, 100));
	ImGui::Begin("File Name", &show_fileName_window, ImGuiWindowFlags_NoResize);

	if (ImGui::InputText("", fileName, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		Engine->SaveScene(fileName);
	}

	if (ImGui::Button("Accept"))
	{
		Engine->SaveScene(fileName);
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
	Engine->fileSystem->SplitFilePath(Engine->scene->current_scene.c_str(), nullptr, &file, &extension);
	std::string str = file;
	if (extension != "")
		file.append("." + extension);
	strcpy_s(fileName, 50, str.c_str());
}

void M_Editor::UpdateFPSData(int fps, int ms)
{
	//TODO: Remove dirty access as the code gets cleaner.
	if (windowFrames[0]->windows[6] != nullptr)
		((W_EngineConfig*)windowFrames[0]->windows[6])->UpdateFPSData(fps, ms);
}

void M_Editor::OnResize(int screen_width, int screen_height)
{
	
	ImGuiContext& g = *ImGui::GetCurrentContext();
	float iconbar_size = 30.0f;
}

bool M_Editor::UsingKeyboard() const
{
	return using_keyboard;
}

bool M_Editor::UsingMouse() const
{
	return using_mouse;
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
			Engine->scene->DeleteGameObject((GameObject*)selectedGameObjects[i]);
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

void M_Editor::SaveConfig(Config& config) const
{
	config.SetBool("Show Debug Info", showDebugInfo);

	/*
	std::vector<Window*>::const_iterator it;
	for (it = windows.begin(); it != windows.end(); ++it)
	{
		(*it)->SaveConfig(config.SetNode((*it)->name.c_str()));
	}
	*/
}

void M_Editor::LoadConfig(Config& config)
{
	showDebugInfo = config.GetBool("Show Debug Info", false);

	/*
	std::vector<Window*>::const_iterator it;
	for (it = windows.begin(); it != windows.end(); ++it)
	{
		(*it)->LoadConfig(config.GetNode((*it)->name.c_str()));
	}
	*/
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