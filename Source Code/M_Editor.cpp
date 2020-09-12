#include "Engine.h"
#include "Config.h"
#include "M_Editor.h"
#include "M_Window.h"

#include "M_Input.h"

//Windows
#include "WF_SceneEditor.h"
#include "WF_ParticleEditor.h"
#include "W_Console.h"
#include "W_EngineConfig.h"

#include "M_Scene.h"
#include "M_FileSystem.h"
#include "M_Resources.h"
#include "M_Renderer3D.h"

#include "R_Scene.h"

#include "GameObject.h"
#include "Resource.h"

#include "OpenGL.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
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
	io.ConfigViewportsNoDecoration = false;
	io.ConfigViewportsNoAutoMerge = true;
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.ConfigDockingTransparentPayload = true;

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

	io.IniFilename = "Engine/imgui.ini";
	io.MouseDrawCursor = false;
	
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	//TODO: not sure if this should be done here too, but it's kinda valid
	LoadConfig(config);

	return true;
}

bool M_Editor::Start()
{
	frameWindowClass = new ImGuiWindowClass();
	frameWindowClass->ClassId = 1;
	frameWindowClass->DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoSplit;

	normalWindowClass = new ImGuiWindowClass();
	normalWindowClass->ClassId = 2;

	if (TryLoadEditorState() == false)
	{
		uint64 sceneID = Engine->moduleResources->FindResourceBase("Engine/Assets/Defaults/Untitled.scene")->ID;
		OpenWindowFromResource(sceneID);
	}

	
	ImGuiIO& io = ImGui::GetIO();
	
	return true;
}

update_status M_Editor::PreUpdate()
{
	for (uint i = 0; i < windowFrames.size(); ++i)
	{
		//Handle any window frame close requests
		if (!windowFrames[i]->IsActive())
		{
			delete windowFrames[i];
			windowFrames.erase(windowFrames.begin() + i);
			--i;
		}
	}

	//Begin new ImGui Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(Engine->window->window);
	ImGui::NewFrame();

	//Update ImGui input usage
	ImGuiIO& io = ImGui::GetIO();
	using_keyboard = io.WantCaptureKeyboard;
	using_mouse = io.WantCaptureMouse;
	
	return UPDATE_CONTINUE;
}


void M_Editor::LoadLayout_Default(WindowFrame* windowFrame)
{
	ImGuiID dockspace_id = ImGui::GetID("Main Dockspace");
	windowFrame->LoadLayout_Default(dockspace_id);
	windowFrame->pendingLoadLayout = false;
}

void M_Editor::LoadLayout(WindowFrame* windowFrame, const char* layout)
{
	//TODO: This is meant to be added when the user can generate new custom layouts
}

void M_Editor::SaveEditorState()
{
	Config file;
	Config_Array arr = file.SetArray("Windows");
	for (uint i = 0; i < windowFrames.size(); ++i)
	{
		Config node = arr.AddNode();
		node.SetNumber("ID", windowFrames[i]->GetID());
		node.SetNumber("Resource ID", windowFrames[i]->GetResourceID());
	}
	char* buffer = nullptr;
	if (uint size = file.Serialize(&buffer))
	{
		Engine->fileSystem->Save("Engine/EditorState.json", buffer, size);
	}
}

bool M_Editor::TryLoadEditorState()
{
	char* buffer = nullptr;
	if (uint size = Engine->fileSystem->Load("Engine/EditorState.json", &buffer))
	{
		Config file(buffer);
		Config_Array arr = file.GetArray("Windows");
		for (uint i = 0; i < arr.GetSize(); ++i)
		{
			Config windowNode = arr.GetNode(i);
			OpenWindowFromResource(windowNode.GetNumber("Resource ID"), windowNode.GetNumber("ID"));
		}
		return true;
	}
	return false;
}

bool M_Editor::IsWindowLayoutSaved(WindowFrame* windowFrame) const
{
	//Windows frames are saved as [Window][###<window_name>_<window_id>]]
	std::string windowTabStr = std::string("[Window][###") + windowFrame->GetName() + "_" + std::to_string(windowFrame->GetID()) + "]";

	//Parsing imgui.ini. We mimic ImGui's logic (function LoadIniSettingsFromMemory in imgui.cpp)
	char* buffer = nullptr;
	if (uint fileSize = Engine->fileSystem->Load("Engine/imgui.ini", &buffer))
	{
		char* buffer_end = buffer + fileSize;
		buffer_end[0] = 0;

		char* line_end = nullptr;
		for (char* line = buffer; line < buffer_end; line = line_end + 1)
		{
			// Skip new lines markers, then find end of the line
			while (*line == '\n' || *line == '\r')
				line++;
			line_end = line;
			while (line_end < buffer_end && *line_end != '\n' && *line_end != '\r')
				line_end++;
			line_end[0] = 0;
			if (line[0] == ';') //Not sure in which situation this would happen
				continue;
			if (line[0] == '[' && line_end > line&& line_end[-1] == ']') //<-- we found a tab entry (window or dock data)
			{
				if (strcmp(windowTabStr.c_str(), line) == 0) //<-- 'line' is the beginning of the line, and 'line_end' was set to '0'
					return true;
			}

		}
		RELEASE_ARRAY(buffer);
	}

	return false;
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

	ImGuiWindowFlags frameWindow_flags =
		ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowClass(frameWindowClass);
	ImGui::Begin("Main Window", nullptr, frameWindow_flags);
	ImGui::PopStyleVar();

	ImGuiID dockspace_id = ImGui::GetID("Main Dockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoSplit, frameWindowClass);

	for (uint i = 0; i < windowFrames.size(); ++i)
		windowFrames[i]->Draw();

	//Layouts are loaded after draw so the draw call on docks does not happen twice in the same frame
	for (uint i = 0; i < windowFrames.size(); ++i)
		if (windowFrames[i]->pendingLoadLayout)
			LoadLayout_Default(windowFrames[i]);

	ImGui::End();

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
	SaveEditorState();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}

void M_Editor::Log(const char* input)
{
	if (windowFrames.size() > 0)
	{
		W_Console* w_console = (W_Console*)GetWindowFrame(WF_SceneEditor::GetName())->GetWindow(W_Console::GetName());
		if (w_console)
			w_console->AddLog(input);
	}
}

void M_Editor::GetEvent(SDL_Event* event)
{
	ImGui_ImplSDL2_ProcessEvent(event);
}

void M_Editor::ShowFileNameWindow()
{
	ImGui::SetNextWindowSize(ImVec2(400, 100));
	ImGui::Begin("File Name", &show_fileName_window, ImGuiWindowFlags_NoResize);

	if (ImGui::InputText("", fileName, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		Engine->moduleResources->SaveResourceAs(Engine->scene->hScene.Get(), "Assets/Scenes", fileName);
		show_fileName_window = false;
	}

	if (ImGui::Button("Accept"))
	{
		Engine->moduleResources->SaveResourceAs(Engine->scene->hScene.Get(), "Assets/Scenes", fileName);
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
	Engine->fileSystem->SplitFilePath(Engine->scene->hScene.Get()->GetAssetsFile(), nullptr, &file, &extension);
	std::string str = file;
	if (extension != "")
		file.append("." + extension);
	strcpy_s(fileName, 50, str.c_str());
}

WindowFrame* M_Editor::GetWindowFrame(const char* name)
{
	for (uint i = 0; i < windowFrames.size(); ++i)
		if (strcmp(windowFrames[i]->GetName(), name) == 0) return windowFrames[i];

	return nullptr;
}

void M_Editor::UpdateFPSData(int fps, int ms)
{
	if (windowFrames.size() > 0)
	{
		W_EngineConfig* w_engineConfig = (W_EngineConfig*)GetWindowFrame(WF_SceneEditor::GetName())->GetWindow(W_EngineConfig::GetName());
		if (w_engineConfig)
			w_engineConfig->UpdateFPSData(fps, ms);
	}
}

void M_Editor::OnResize(int screen_width, int screen_height)
{
	ImGuiContext& g = *ImGui::GetCurrentContext();
	float iconbar_size = 30.0f;
}

bool M_Editor::OpenWindowFromResource(uint64 resourceID, uint64 forceWindowID)
{
	ResourceHandle<Resource> newResourceHandle(resourceID);
	Resource* resource = newResourceHandle.Get();
	if (resource == nullptr) return false;

	WindowFrame* windowFrame = nullptr;

	switch (resource->GetType())
	{
		case(ResourceType::SCENE):
		{
			if (windowFrame = GetWindowFrame(WF_SceneEditor::GetName()))
				{ windowFrame->SetResource(resourceID); return true; }
			else
				windowFrame = new WF_SceneEditor(this, frameWindowClass, normalWindowClass, forceWindowID ? forceWindowID : random.Int());
			break;
		}
		case (ResourceType::MATERIAL):
		{
			//Yeah, I wish (:
			break;
		}
		case(ResourceType::PARTICLESYSTEM):
		{
			windowFrame = new WF_ParticleEditor(this, frameWindowClass, normalWindowClass, forceWindowID ? forceWindowID : random.Int());
			break;
		}
	}
	if (windowFrame != nullptr)
	{
		windowFrame->SetResource(resourceID);
		windowFrames.push_back(windowFrame);

		//If the window layout is not saved in ImGui.ini file, we will load the default layout in the next frame
		if (forceWindowID == 0 || !IsWindowLayoutSaved(windowFrame))
			windowFrame->pendingLoadLayout = true;
	}
}

void M_Editor::CloseWindow(WindowFrame* windowFrame)
{
	windowFrames.erase(std::find(windowFrames.begin(), windowFrames.end(), windowFrame));
	RELEASE(windowFrame);
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
	hSelectedResource.Set((uint64)0);
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
			//Engine->moduleResources->
		}
	}
	selectedGameObjects.clear();
	UnselectResources();
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

void M_Editor::LoadScene(const char* path, bool tmp)
{
	ResetScene();

	uint64 sceneID = Engine->scene->LoadScene(path);

	WF_SceneEditor* sceneEditor = (WF_SceneEditor*)GetWindowFrame(WF_SceneEditor::GetName());
	sceneEditor->SetResource(sceneID);
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

void M_Editor::OnViewportClosed(uint32_t SDLWindowID)
{
	ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle((void*)SDL_GetWindowFromID(SDLWindowID));
	
	for (uint i = 0u; i < windowFrames.size(); ++i)
	{
		ImGuiWindow* window = ImGui::FindWindowByName(windowFrames[i]->GetWindowStrID());
		if (window->Viewport == viewport)
		{
			//This function will be called from Input's PreUpdate so it's safe to destroy the windows here
			CloseWindow(windowFrames[i]);
			--i;
		}
	}

}

//------------------------------------