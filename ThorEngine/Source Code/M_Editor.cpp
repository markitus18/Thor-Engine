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

#include "M_SceneManager.h"
#include "M_FileSystem.h"
#include "M_Resources.h"
#include "M_Renderer3D.h"

#include "I_Scenes.h"
#include "R_Folder.h"
#include "R_Map.h"
#include "Scene.h"

#include "GameObject.h"
#include "Resource.h"

#include "OpenGL.h"

#define IMGUI_DEFINE_MATH_OPERATORS
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
	
	//TODO: not sure if this should be done here too, but it's kinda valid
	LoadConfig(config);

	return true;
}

bool M_Editor::Start()
{
	//TODO: Add different windowClasses for every windowFrame.
	//This way only windows belonging to the same windowFrame can dock into it  
	frameWindowClass = new ImGuiWindowClass();
	frameWindowClass->ClassId = 1;
	frameWindowClass->DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoSplit;

	normalWindowClass = new ImGuiWindowClass();
	normalWindowClass->ClassId = 2;

	if (TryLoadEditorState() == false)
	{
		uint64 sceneID = Engine->moduleResources->FindResourceBase("Engine/Assets/Defaults/Untitled.map")->ID;
		OpenWindowFromResource(sceneID);
	}

	
	ImGuiIO& io = ImGui::GetIO();
	
	return true;
}

update_status M_Editor::PreUpdate()
{
	if (userRequestedExit)
		return UPDATE_STOP;

	for (uint i = 0; i < windowFrames.size(); ++i)
	{
		windowFrames[i]->PrepareUpdate();
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
		uint64 windowID = windowFrames[i]->GetID();
		uint64 resourceID = windowFrames[i]->GetResourceID();

		node.Serialize("ID", windowID);
		node.Serialize("Resource ID", resourceID);
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
			uint64 resourceID, windowID;

			windowNode.Serialize("Resource ID", resourceID);
			windowNode.Serialize("ID", windowID);
			OpenWindowFromResource(resourceID, windowID);
		}
		return true;
	}
	return false;
}

bool M_Editor::IsWindowLayoutSaved(WindowFrame* windowFrame) const
{
	//Windows frames are saved as [Window][###<window_name>_<window_id>]]
	std::string windowStrID = std::string("[Window][###") + windowFrame->GetName() + ("_") + std::to_string(windowFrame->GetID());

	if (!Engine->fileSystem->Exists("Engine/imgui.ini")) return false;

	//Parsing imgui.ini. We mimic ImGui's logic (function LoadIniSettingsFromMemory in imgui.cpp)
	char* buffer = nullptr;
	if (uint fileSize = Engine->fileSystem->Load("Engine/imgui.ini", &buffer))
	{
		char* buffer_end = buffer + fileSize - 1;

		char* line = buffer;
		char* line_end = nullptr;
		while (Ini_FindNextEntry(line, line_end, buffer_end))
		{
			if (strncmp(windowStrID.c_str(), line, windowStrID.size()) == 0) //<-- 'line' is the beginning of the line, and 'line_end' was set to '0'
				return true;
			line = line_end + 1;
		}
		RELEASE_ARRAY(buffer);
	}

	return false;
}

bool M_Editor::DoesAnyWindowNeedSave() const
{
	for (uint i = 0; i < windowFrames.size(); ++i)
	{
		ResourceHandle<Resource> windowResource = windowFrames[i]->GetResourceID();
		if (windowResource.Get() && windowResource.Get()->needs_save)
		{
			return true;
		}
	}
	return false;
}

void M_Editor::ClearWindowFromIni(WindowFrame* windowFrame)
{
	const std::vector<Window*> childWindows = windowFrame->GetWindows();
	std::string winStrID = std::string("###") + windowFrame->GetName() + ("_") + std::to_string(windowFrame->GetID());

	//Remove window and dock data from imgui.ini file
	if (!Engine->fileSystem->Exists("Engine/imgui.ini")) return;

	char* buffer = nullptr;
	uint fileSize = Engine->fileSystem->Load("Engine/imgui.ini", &buffer);

	//Parsing imgui.ini. We mimic ImGui's logic (function LoadIniSettingsFromMemory in imgui.cpp)
	char* buffer_end = buffer + fileSize - 1;

	char* line_end = nullptr;
	char* line = buffer;

	while (Ini_FindNextEntry(line, line_end, buffer_end))
	{
		if (strncmp(line, "[Window]", 8) == 0) //Found a window entry! Iterate all windows to find a matching ID
		{
			//TODO: This could be simplified by checking only for ID!
			bool matchingEntry = false;
			if (strncmp(line + 9, winStrID.c_str(), (line_end - 1) - (line + 9)) == 0)
				matchingEntry = true;
			for (uint i = 0u; i < childWindows.size() && !matchingEntry; ++i)
			{
				if (strncmp(line + 9, childWindows[i]->GetWindowStrID(), (line_end - 1) - (line + 9)) == 0)
					matchingEntry = true;
			}
			if (matchingEntry)
			{
				char* next_line = line_end + 1;
				char* next_line_end = line_end;

				Ini_FindNextEntry(next_line, next_line_end, buffer_end); //<-- no need to check return as dock entry will always be after windows
				//Remove from current entry until next entry
				uint removeSize = next_line - line;
				for (uint i = 0u; next_line + i <= buffer_end; ++i)
					line[i] = next_line[i];

				fileSize -= removeSize;
				buffer_end -= removeSize;
				buffer_end[1] = '\0';
			}
			else
				line = line_end + 1;
		}
		else if (strncmp(line, "[Docking]", 9) == 0) //Found the docking entry
		{
			while (Ini_FindNextDockEntry(line, line_end, buffer_end))
			{
				char* cursor = line + strlen("DockSpace");
				while (*cursor == ' ' || *cursor == '\t')
					cursor++;

				uint ID, windowID;
				int r;
				if (sscanf(cursor, "ID=0x%08X Window=0x%08X%n", &ID, &windowID, &r) != 2) { line = line_end + 1; continue; }

				bool matchingEntry = false;
				ImGuiWindow* window = ImGui::FindWindowByID(windowID);
				if (window && strcmp(window->Name, windowFrame->GetWindowStrID()) == 0)
					matchingEntry = true;

				for (uint i = 0u; i < childWindows.size() && !matchingEntry; ++i)
				{
					ImGuiWindow* childWindow = ImGui::FindWindowByID(windowID);
					if (childWindow && strcmp(childWindow->Name, childWindows[i]->GetWindowStrID()) == 0)
						matchingEntry = true;
				}
				if (matchingEntry)
				{
					char* next_line = line_end + 1;
					char* next_line_end = line_end;

					if (Ini_FindNextDockEntry(next_line, next_line_end, buffer_end))
					{

						//Remove from current entry until next entry
						uint removeSize = next_line - line;
						for (uint i = 0u; next_line + i <= buffer_end; ++i)
							line[i] = next_line[i];

						fileSize -= removeSize;
						buffer_end -= removeSize;
						buffer_end[1] = '\0';
					}
					else //End of file!
					{
						line[0] = '\n';
						line[1] = '\n';
						line[2] = '\0';

						uint removeSize = (buffer_end - line);
						fileSize -= removeSize;
						buffer_end -= removeSize;
					}
				}
				else
					line = line_end + 1;
			}
		}
		else
			line = line_end + 1;
	}

	Engine->fileSystem->Save("Engine/imgui.ini", buffer, fileSize);
	RELEASE_ARRAY(buffer);
}

bool M_Editor::Ini_FindNextEntry(char*& line, char*& line_end, char*& buffer_end) const
{
	while (line_end < buffer_end)
	{
		// Skip new lines markers, then find end of the line
		while (*line == '\n' || *line == '\r')
			line++;
		line_end = line;
		while (line_end < buffer_end && *line_end != '\n' && *line_end != '\r')
			line_end++;

		if (line[0] == '[' && line_end > line && line_end[-1] == ']')
			return true;
		else
			line = line_end + 1;
	}
	return false;
}

bool M_Editor::Ini_FindNextDockEntry(char*& line, char*& line_end, char*& buffer_end) const
{
	// Skip new lines markers, then find end of the line
	while (line_end < buffer_end)
	{
		while (*line == '\n' || *line == '\r' || *line == ' ' || *line == '\t') //<-- here we also skip blanks
			line++;
		line_end = line;
		while (line_end < buffer_end && *line_end != '\n' && *line_end != '\r')
			line_end++;
		if (strncmp(line, "DockSpace", 9) == 0)
			return true;
		else
			line = line_end + 1;
	}	
	return false;
}

void M_Editor::Draw()
{
	if (Engine->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		DeleteSelected();
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

	// We iterate in reverse so that we can handle the main window closure last
	// and can remove windows from the array without the need of moving the iterator
	for (int i = windowFrames.size() - 1; i >= 0; --i)
	{
		windowFrames[i]->Draw();
		if (windowFrames[i]->requestClose)
		{
			RequestWindowClose(windowFrames[i]);
		}
	}

	if (show_fileName_window)
		ShowFileNameWindow();

	if (show_Demo_window)
		ImGui::ShowDemoWindow(&show_Demo_window);

	if (windowPendingDelete)
		ShowWindowCloseConfirmation();

	//Layouts are loaded after draw so the draw call on docks does not happen twice in the same frame
	for (uint i = 0; i < windowFrames.size(); ++i)
	{
		if (windowFrames[i]->pendingLoadLayout)
			LoadLayout_Default(windowFrames[i]);
	}

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

void M_Editor::ProcessSDLEvent(const SDL_Event& event)
{
	if (event.window.event != SDL_WINDOWEVENT_CLOSE)
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
	}
	else
	{
		// If the user requests to close a window frame we check if there are any changes to be saved
		if (WindowFrame* windowFrame = GetWindowFromSDLID(event.window.windowID))
		{
			windowFrame->requestClose = true;
		}
		// Otherwise it is a window and we close it normally
		else
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
		}
	}
}

void M_Editor::ShowFileNameWindow()
{
	ImGui::SetNextWindowSize(ImVec2(400, 100));
	ImGui::Begin("File Name", &show_fileName_window, ImGuiWindowFlags_NoResize);

	bool saveFile = false;
	if (ImGui::InputText("", fileName, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
		saveFile = true;

	if (ImGui::Button("Accept"))
		saveFile = true;

	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
	{
		show_fileName_window = false;
	}

	if (saveFile)
	{
		Engine->sceneManager->SaveCurrentScene(fileName);
		show_fileName_window = false;
	}
	ImGui::End();
}

void M_Editor::ShowWindowCloseConfirmation()
{
	ImGuiWindow* window = ImGui::FindWindowByName(windowPendingDelete->GetWindowStrID());
	ImGui::SetNextWindowViewport(window->ViewportId);
	ImGui::SetNextWindowPos(window->ViewportPos + window->Viewport->Size / 2.0f, 0, ImVec2(0.5f, 0.5f));
	
	if (ImGui::BeginPopupModal("Close confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
	{
		ImGui::Text("There might be unsaved changes, are you sure you want to quit?");

		if (windowPendingDelete == windowFrames[0])
		{
			ImGui::Separator();
			ImGui::NewLine();
			for (uint i = 0; i < windowFrames.size(); ++i)
			{
				ResourceHandle<Resource> windowResource = windowFrames[i]->GetResourceID();
				if (windowResource.Get() && windowResource.Get()->needs_save)
				{
					ImGui::Checkbox(windowResource.Get()->GetName(), &windowFrames[i]->checkedForSave);
				}
			}
		}

		ImGui::NewLine();
		ImGui::NewLine();

		if (ImGui::Button("Save & Close"))
		{
			if (windowPendingDelete == windowFrames[0])
			{
				for (uint i = 0; i < windowFrames.size(); ++i)
				{
					ResourceHandle<Resource> windowResource = windowFrames[i]->GetResourceID();
					if (windowResource.Get() && windowResource.Get()->needs_save && windowFrames[i]->checkedForSave)
					{
						Engine->moduleResources->SaveResource(windowResource.Get());
					}
				}
			}
			else
			{
				ResourceHandle<Resource> windowResource(windowPendingDelete->GetResourceID());
				Engine->moduleResources->SaveResource(windowResource.Get());
			}

			CloseWindow(windowPendingDelete);
			windowPendingDelete = nullptr;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Close"))
		{
			CloseWindow(windowPendingDelete);
			windowPendingDelete = nullptr;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			windowPendingDelete->requestClose = false;
			windowFrames[0]->requestClose = false;
			windowPendingDelete = nullptr;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void M_Editor::OpenFileNameWindow()
{
	show_fileName_window = true;
	strcpy_s(fileName, 50, Engine->sceneManager->mainScene->name.c_str());
}

WindowFrame* M_Editor::GetWindowFrame(const char* name) const
{
	for (uint i = 0; i < windowFrames.size(); ++i)
		if (strcmp(windowFrames[i]->GetName(), name) == 0) return windowFrames[i];

	return nullptr;
}

WindowFrame* M_Editor::GetWindowFromSDLID(uint32_t SDLWindowID) const
{
	ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle((void*)SDL_GetWindowFromID(SDLWindowID));

	for (uint i = 0u; i < windowFrames.size(); ++i)
	{
		ImGuiWindow* window = ImGui::FindWindowByName(windowFrames[i]->GetWindowStrID());
		if (window->Viewport == viewport)
		{
			return windowFrames[i];
		}
	}
	return nullptr;
}

const char* M_Editor::GetCurrentExplorerDirectory() const
{
	return hExplorerFolder.Get()->GetAssetsFile();
}

void M_Editor::UpdateFPSData(int fps, int ms)
{
	if (windowFrames.size() > 0)
	{
		WindowFrame* mainWindowFrame = GetWindowFrame(WF_SceneEditor::GetName());
		W_EngineConfig* w_engineConfig = mainWindowFrame ? (W_EngineConfig*)mainWindowFrame->GetWindow(W_EngineConfig::GetName()) : nullptr;
		if (w_engineConfig)
			w_engineConfig->UpdateFPSData(fps, ms);
	}

	// Quite a dirty way of sending the data. Re-structure time accessing.
	// FPS and ms should be accesed through Time:: and we can avoid calling this function. Update in w_engineConfig draw itself
	for (uint i = 0; i < Engine->sceneManager->activeScenes.size(); ++i)
	{
		Engine->sceneManager->activeScenes[i]->stats.FPS = fps;
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
	uint64 newWindowID = forceWindowID ? forceWindowID : random.Int();

	switch (resource->GetType())
	{
		case(ResourceType::MAP):
		{
			if (windowFrame = GetWindowFrame(WF_SceneEditor::GetName()))
			{
				windowFrame->SetResource(resourceID);
				return true;
			}
			else
			{
				windowFrame = new WF_SceneEditor(this, frameWindowClass, normalWindowClass, newWindowID);
			}
			break;
		}
		case (ResourceType::MATERIAL):
		{
			//Yeah, I wish (:
			break;
		}
		case(ResourceType::PARTICLESYSTEM):
		{
			windowFrame = new WF_ParticleEditor(this, frameWindowClass, normalWindowClass, newWindowID);
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

		return true;
	}
	return false;
}

void M_Editor::RequestWindowClose(WindowFrame* windowFrame)
{
	if (!windowPendingDelete && windowFrame)
	{
		ResourceHandle<Resource> windowResource(windowFrame->GetResourceID());
		if (windowResource.Get() && windowResource.Get()->needs_save || windowFrame == windowFrames[0] && DoesAnyWindowNeedSave())
		{
			windowPendingDelete = windowFrame;
			ImGui::OpenPopup("Close confirmation");
		}
		else
		{
			CloseWindow(windowFrame);
		}
	}
}

void M_Editor::CloseWindow(WindowFrame* windowFrame)
{
	if (windowFrame == windowFrames[0])
	{
		userRequestedExit = true;
	}
	else
	{
		//TODO: Remove data from ImGui's memory. The library will later override the file.
		ClearWindowFromIni(windowFrame);
		windowFrames.erase(std::find(windowFrames.begin(), windowFrames.end(), windowFrame));
		RELEASE(windowFrame);
	}
}

void M_Editor::OnUserExitRequest()
{
	windowFrames[0]->requestClose = true;
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
	for (uint i = 0; i < selectedGameObjects.size(); /**/)
	{
		selectedGameObjects[i]->Unselect();
		if (selectedGameObjects[i]->GetType() == GAMEOBJECT)
		{
			((GameObject*)selectedGameObjects[i])->Destroy();
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

void M_Editor::LoadMap(const char* path, bool tmp)
{
	ResetScene();

	Engine->sceneManager->LoadMap(path);

	WF_SceneEditor* sceneEditor = (WF_SceneEditor*)GetWindowFrame(WF_SceneEditor::GetName());
	sceneEditor->SetResource(Engine->sceneManager->mainScene->ID);
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