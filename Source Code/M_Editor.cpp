#include "Engine.h"
#include "Config.h"
#include "M_Editor.h"
#include "M_Window.h"

#include "M_Input.h"

//Windows
#include "W_Scene.h"
#include "W_Hierarchy.h"
#include "W_Inspector.h"
#include "W_Explorer.h"
#include "W_Console.h"
#include "W_Resources.h"
#include "W_EngineConfig.h"
#include "W_ParticleEditor.h"

#include "M_Scene.h"
#include "M_FileSystem.h"
#include "M_Resources.h"
#include "M_Camera3D.h"
#include "M_Renderer3D.h"

#include "GameObject.h"
#include "Resource.h"

#include "OpenGL.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_internal.h"
#include "ImGuizmo/ImGuizmo.h"

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
	CreateWindows();
	
	w_explorer->resourceIcons[Resource::Type::FOLDER] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/FolderIcon.png").ID;
	w_explorer->resourceIcons[Resource::Type::MESH] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/FileIcon.png").ID;
	w_explorer->resourceIcons[Resource::Type::TEXTURE] = 0;
	w_explorer->resourceIcons[Resource::Type::MATERIAL] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/MaterialIcon.png").ID;
	w_explorer->resourceIcons[Resource::Type::ANIMATION] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/AnimationIcon.png").ID;
	w_explorer->resourceIcons[Resource::Type::ANIMATOR_CONTROLLER] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/AnimatorIcon.png").ID;
	w_explorer->resourceIcons[Resource::Type::PREFAB] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/SceneIcon.png").ID;
	w_explorer->resourceIcons[Resource::Type::PARTICLESYSTEM] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/ParticlesIcon.png").ID;
	w_explorer->resourceIcons[Resource::Type::SHADER] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/ShaderIcon.png").ID;
	w_explorer->resourceIcons[Resource::Type::SCENE] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/ThorIcon.png").ID;
	w_explorer->selectedResourceImage = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/SelectedIcon.png").ID;
	
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
	w_hierarchy = new W_Hierarchy(this);
	windows.push_back(w_hierarchy);

	w_scene = new W_Scene(this);
	windows.push_back(w_scene);

	w_inspector = new W_Inspector(this);
	windows.push_back(w_inspector);

	w_particles = new W_ParticleEditor(this);
	windows.push_back(w_particles);

	w_explorer = new W_Explorer(this);
	windows.push_back(w_explorer);

	w_console = new W_Console(this);
	windows.push_back(w_console);

	w_resources = new W_Resources(this);
	windows.push_back(w_resources);

	w_econfig = new W_EngineConfig(this);
	windows.push_back(w_econfig);

	LoadLayout();
}

void M_Editor::LoadLayout()
{
	//We generate a fake ImGui frame in order to load the layout correctly and set up all docking data
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(Engine->window->window);
	ImGui::NewFrame();

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::Begin("DockSpace Demo", nullptr, 0);

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);

	ImGuiID leftSpace_id, rightspace_id;
	ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.2f, &rightspace_id, &leftSpace_id);

	ImGuiID topRightSpace_id, bottomRightSpace_id;
	ImGui::DockBuilderSplitNode(rightspace_id, ImGuiDir_Up, 0.7f, &topRightSpace_id, &bottomRightSpace_id);

	ImGui::DockBuilderDockWindow("Inspector", topRightSpace_id);
	ImGui::DockBuilderDockWindow("Engine Config", bottomRightSpace_id);
	ImGui::DockBuilderDockWindow("Resources", bottomRightSpace_id);

	ImGuiID topLeftSpace_id, bottomLeftSpace_id;
	ImGui::DockBuilderSplitNode(leftSpace_id, ImGuiDir_Up, 0.7f, &topLeftSpace_id, &bottomLeftSpace_id);

	ImGui::DockBuilderDockWindow("Explorer", bottomLeftSpace_id);
	ImGui::DockBuilderDockWindow("Console", bottomLeftSpace_id);

	ImGuiID leftTopLeftSpace_id, rightTopLeftSpace_id;
	ImGui::DockBuilderSplitNode(topLeftSpace_id, ImGuiDir_Right, 0.8f, &rightTopLeftSpace_id, &leftTopLeftSpace_id);

	ImGui::DockBuilderDockWindow("Hierarchy", leftTopLeftSpace_id);
	ImGui::DockBuilderDockWindow("Scene", rightTopLeftSpace_id);

	ImGui::DockBuilderFinish(dockspace_id);

	ImGui::End();
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


	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
	window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", nullptr, window_flags);
	ImGui::PopStyleVar();

	ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	ImGuiID dockspace_id = 0;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	static bool doOnce = true;
	if (doOnce)
	{
	//	LoadLayout();
		doOnce = false;
	}

//	ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockspace_id);
//	node->HostWindow->name

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
				Engine->scene->CreateDefaultScene();
			}

			if (ImGui::BeginMenu("Open Scene"))
			{
				//TODO: avoid doing this every frame
				sceneList.clear();
				Engine->fileSystem->GetAllFilesWithExtension("", "scene", sceneList);

				for (uint i = 0; i < sceneList.size(); i++)
				{
					if (ImGui::MenuItem(sceneList[i].c_str()))
					{
						Engine->LoadScene(sceneList[i].c_str());
					}
				}
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Save Scene"))
			{
				if (Engine->scene->current_scene == "Untitled")
				{
					OpenFileNameWindow();
				}
				else
				{
					Engine->SaveScene(Engine->scene->current_scene.c_str());
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
					Engine->scene->CreateCamera();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::MenuItem("Create Empty"))
			{
				std::string name(Engine->scene->GetNewGameObjectName("GameObject"));
				GameObject* newGameObject = Engine->scene->CreateGameObject(name.c_str());
				SelectSingle(newGameObject);
			}

			if (ImGui::MenuItem("Create Empty Child"))
			{
				GameObject* parent = (GameObject*)(selectedGameObjects.size() > 0 ? selectedGameObjects[0] : nullptr);
				std::string name(Engine->scene->GetNewGameObjectName("GameObject", parent));
				GameObject* newGameObject = Engine->scene->CreateGameObject(name.c_str(), parent);
				SelectSingle(newGameObject);
			}

			if (ImGui::MenuItem("Create Empty x10"))
			{
				for (uint i = 0; i < 10; i++)
				{
					std::string name(Engine->scene->GetNewGameObjectName("GameObject"));
					GameObject* newGameObject = Engine->scene->CreateGameObject(name.c_str());
					SelectSingle(newGameObject);
				}
			}
			if (ImGui::BeginMenu("3D Object"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					std::string name(Engine->scene->GetNewGameObjectName("Cube"));
					GameObject* newGameObject = Engine->scene->CreateGameObject(name.c_str());
					SelectSingle(newGameObject);

				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::Checkbox("Show All Debug Info", &showDebugInfo);

			std::vector<DWindow*>::iterator it;
			for (it = windows.begin(); it != windows.end(); ++it)
			{
				if (ImGui::BeginMenu((*it)->name.c_str()))
				{
					ImGui::Checkbox("Show Debug Info", &(*it)->showDebugInfo);
					ImGui::EndMenu();
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About Thor Engine   ", nullptr, &show_About_window);
			ImGui::Separator();
			if (ImGui::MenuItem("Documentation       "))
			{
				Engine->RequestBrowser("https://github.com/markitus18/Game-Engine/wiki");
			}
			if (ImGui::MenuItem("Download latest     "))
			{
				Engine->RequestBrowser("https://github.com/markitus18/Game-Engine/releases");
			}
			if (ImGui::MenuItem("Report a bug        "))
			{
				Engine->RequestBrowser("https://github.com/markitus18/Game-Engine/issues");
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debugging Tools"))
		{
			ImGui::MenuItem("ImGui Demo", nullptr, &show_Demo_window);
			if (ImGui::BeginMenu("Display"))
			{
				ImGui::MenuItem("Quadtree", nullptr, &Engine->scene->drawQuadtree);
				ImGui::MenuItem("Ray picking", nullptr, &Engine->camera->drawRay);
				ImGui::MenuItem("GameObjects box", nullptr, &Engine->scene->drawBounds);
				ImGui::MenuItem("GameObjects box (selected)", nullptr, &Engine->scene->drawBoundsSelected);
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();

	for (uint i = 0; i < windows.size(); ++i)
	{
		if (windows[i]->IsActive())
			windows[i]->Draw();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
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
	if (w_console != nullptr)
		w_console->AddLog(input);
}

void M_Editor::GetEvent(SDL_Event* event)
{
	ImGui_ImplSDL2_ProcessEvent(event);
}

void M_Editor::ShowAboutWindow()
{
	ImGui::SetNextWindowSize(ImVec2(600, 100));
	ImGui::Begin("About Thor Engine", &show_About_window, ImGuiWindowFlags_NoResize);
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
	if (w_econfig)
		w_econfig->UpdateFPSData(fps, ms);
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

	std::vector<DWindow*>::const_iterator it;
	for (it = windows.begin(); it != windows.end(); ++it)
	{
		(*it)->SaveConfig(config.SetNode((*it)->name.c_str()));
	}
}

void M_Editor::LoadConfig(Config& config)
{
	showDebugInfo = config.GetBool("Show Debug Info", false);

	std::vector<DWindow*>::const_iterator it;
	for (it = windows.begin(); it != windows.end(); ++it)
	{
		(*it)->LoadConfig(config.GetNode((*it)->name.c_str()));
	}
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