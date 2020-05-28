#include "WF_MainWindow.h"

#include "Engine.h"
#include "Globals.h"

#include "M_Scene.h"
#include "M_FileSystem.h"
#include "M_Editor.h"
#include "M_Camera3D.h"

#include "Window.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include "W_Scene.h"
#include "W_Hierarchy.h"
#include "W_Inspector.h"
#include "W_Explorer.h"
#include "W_Console.h"
#include "W_Resources.h"
#include "W_EngineConfig.h"
#include "W_About.h"
#include "W_MainToolbar.h"

WF_MainWindow::WF_MainWindow(M_Editor* editor)
{
	name = "Scene Window"; //TODO: Cannot be initialized in constructor, create constructor paramter

	w_hierarchy = new W_Hierarchy(editor);
	windows.push_back(w_hierarchy);

	w_scene = new W_Scene(editor);
	windows.push_back(w_scene);

	w_inspector = new W_Inspector(editor);
	windows.push_back(w_inspector);

	w_explorer = new W_Explorer(editor);
	windows.push_back(w_explorer);

	w_console = new W_Console(editor);
	windows.push_back(w_console);

	w_resources = new W_Resources(editor);
	windows.push_back(w_resources);

	w_econfig = new W_EngineConfig(editor);
	windows.push_back(w_econfig);

	w_about = new W_About(editor);
	w_about->SetActive(false);
	windows.push_back(w_about);

	w_toolbar = new W_MainToolbar(editor);
	windows.push_back(w_toolbar);

}

WF_MainWindow::~WF_MainWindow()
{
	for (uint i = 0; i < windows.size(); ++i)
		delete windows[i];

	windows.clear();
}

void WF_MainWindow::DrawMenuBar()
{
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
				std::vector<std::string> sceneList;
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
					Engine->moduleEditor->OpenFileNameWindow();
				}
				else
				{
					Engine->SaveScene(Engine->scene->current_scene.c_str());
				}
			}

			if (ImGui::MenuItem("Save Scene as"))
			{
				Engine->moduleEditor->OpenFileNameWindow();
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
				Engine->moduleEditor->SelectSingle((TreeNode*)newGameObject);
			}

			if (ImGui::MenuItem("Create Empty Child"))
			{
				GameObject* parent = (GameObject*)(Engine->moduleEditor->selectedGameObjects.size() > 0 ? Engine->moduleEditor->selectedGameObjects[0] : nullptr);
				std::string name(Engine->scene->GetNewGameObjectName("GameObject", parent));
				GameObject* newGameObject = Engine->scene->CreateGameObject(name.c_str(), parent);
				Engine->moduleEditor->SelectSingle((TreeNode*)newGameObject);
			}

			if (ImGui::MenuItem("Create Empty x10"))
			{
				for (uint i = 0; i < 10; i++)
				{
					std::string name(Engine->scene->GetNewGameObjectName("GameObject"));
					GameObject* newGameObject = Engine->scene->CreateGameObject(name.c_str());
					Engine->moduleEditor->SelectSingle((TreeNode*)newGameObject);
				}
			}
			if (ImGui::BeginMenu("3D Object"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					std::string name(Engine->scene->GetNewGameObjectName("Cube"));
					GameObject* newGameObject = Engine->scene->CreateGameObject(name.c_str());
					Engine->moduleEditor->SelectSingle((TreeNode*)newGameObject);

				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::Checkbox("Show All Debug Info", &Engine->moduleEditor->showDebugInfo);

			std::vector<Window*>::iterator it;
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
			bool aboutActive = w_about->IsActive();
			if (ImGui::MenuItem("About Thor Engine   ", nullptr, &aboutActive))
				w_about->SetActive(aboutActive);

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

		if (ImGui::BeginMenu("Development"))
		{
			ImGui::MenuItem("ImGui Demo", nullptr, &Engine->moduleEditor->show_Demo_window);
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
}