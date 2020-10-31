#include "WF_SceneEditor.h"

#include "Engine.h"
#include "Globals.h"

#include "M_SceneManager.h"
#include "M_Editor.h"
#include "M_Camera3D.h"
#include "M_Resources.h"
#include "M_FileSystem.h"
#include "I_Scenes.h"

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

#include "Scene.h"
#include "R_Map.h"

WF_SceneEditor::WF_SceneEditor(M_Editor* editor, ImGuiWindowClass* frameWindowClass, ImGuiWindowClass* windowClass, int ID) : WindowFrame(GetName(), frameWindowClass, windowClass, ID)
{
	isDockable = false;

	explorerWindowClass = new ImGuiWindowClass();
	explorerWindowClass->ClassId = 3;

	windows.push_back(new W_Hierarchy(editor, windowClass, ID));
	windows.push_back(new W_MainViewport(editor, windowClass, ID));
	windows.push_back(new W_Inspector(editor, windowClass, ID));
	windows.push_back(new W_Console(editor, windowClass, ID));
	windows.push_back(new W_Explorer(editor, windowClass, explorerWindowClass, ID));
	windows.push_back(new W_Resources(editor, windowClass, ID));
	windows.push_back(new W_EngineConfig(editor, windowClass, ID));
	windows.push_back(new W_MainToolbar(editor, windowClass, ID));

	//W_About* w_about = new W_About(editor, windowClass, ID);
	//w_about->SetActive(false);
	//windows.push_back(w_about);
}

WF_SceneEditor::~WF_SceneEditor()
{

}

void WF_SceneEditor::SetResource(uint64 resourceID)
{
	WindowFrame::SetResource(resourceID);
	if (!scene || scene->ID != resourceID)
	{
		Engine->sceneManager->LoadMap(resourceID);
		scene = Engine->sceneManager->mainScene;

		W_MainViewport* viewport = (W_MainViewport*)GetWindow(W_MainViewport::GetName());
		viewport->SetScene(scene);
	}
}

void WF_SceneEditor::MenuBar_File()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New Map"))
		{
			Engine->moduleEditor->LoadMap("Engine/Assets/Defaults/Untitled.map");
		}

		if (ImGui::BeginMenu("Open Map"))
		{
			//FIXME: avoid doing this every frame
			std::vector<std::string> sceneList;
			sceneList.clear();
			Engine->fileSystem->GetAllFilesWithExtension("", "scene", sceneList);

			for (uint i = 0; i < sceneList.size(); i++)
			{
				if (ImGui::MenuItem(sceneList[i].c_str()))
				{
					Engine->sceneManager->LoadMap(sceneList[i].c_str());
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Save Map"))
		{
			if (scene->file_path == "Engine/Assets/Defaults/Untitled.map")
			{
				Engine->moduleEditor->OpenFileNameWindow();
			}
			else
			{
				//TODO: Call scene manager to avoid importer dependeny
				ResourceHandle<R_Map> map(scene->ID);
				Importer::Maps::SaveRootToMap(scene->root, map.Get());
				Engine->moduleResources->SaveResource(map.Get());
			}
		}

		if (ImGui::MenuItem("Save Scene as"))
		{
			Engine->moduleEditor->OpenFileNameWindow();
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Exit          ", nullptr, nullptr, false))
		{
			return;
		}
		ImGui::EndMenu();

	}
}

void WF_SceneEditor::MenuBar_Custom()
{
	if (ImGui::BeginMenu("Create"))
	{
		if (ImGui::MenuItem("Empty"))
		{
			std::string name(scene->GetNewGameObjectName("GameObject"));
			GameObject* newGameObject = scene->CreateNewGameObject(name.c_str());
			Engine->moduleEditor->SelectSingle((TreeNode*)newGameObject);
		}

		if (ImGui::MenuItem("Empty Child"))
		{
			GameObject* parent = (GameObject*)(Engine->moduleEditor->selectedGameObjects.size() > 0 ? Engine->moduleEditor->selectedGameObjects[0] : nullptr);
			std::string name(scene->GetNewGameObjectName("GameObject", parent));
			GameObject* newGameObject = scene->CreateNewGameObject(name.c_str(), parent);
			Engine->moduleEditor->SelectSingle((TreeNode*)newGameObject);
		}

		if (ImGui::MenuItem("Empty Child x10"))
		{
			for (uint i = 0; i < 10; i++)
			{
				std::string name(scene->GetNewGameObjectName("GameObject"));
				GameObject* newGameObject = scene->CreateNewGameObject(name.c_str());
				Engine->moduleEditor->SelectSingle((TreeNode*)newGameObject);
			}
		}
		if (ImGui::BeginMenu("3D Object"))
		{
			if (ImGui::MenuItem("Cube"))
			{
				std::string name(scene->GetNewGameObjectName("Cube"));
				GameObject* newGameObject = scene->CreateNewGameObject(name.c_str());
				Engine->moduleEditor->SelectSingle((TreeNode*)newGameObject);

			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Camera"))
		{
			scene->CreateCamera();
		}
		ImGui::EndMenu();
	}
}

void WF_SceneEditor::MenuBar_Development()
{
	if (ImGui::BeginMenu("Development"))
	{
		ImGui::MenuItem("ImGui Demo", nullptr, &Engine->moduleEditor->show_Demo_window);
		if (ImGui::BeginMenu("Display"))
		{
			ImGui::MenuItem("Quadtree", nullptr, &Engine->sceneManager->drawQuadtree);
			ImGui::MenuItem("Ray picking", nullptr, &Engine->camera->drawRay);
			ImGui::MenuItem("GameObjects box", nullptr, &Engine->sceneManager->drawBounds);
			ImGui::MenuItem("GameObjects box (selected)", nullptr, &Engine->sceneManager->drawBoundsSelected);
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
}

void WF_SceneEditor::LoadLayout_Default(ImGuiID mainDockID)
{
	// Generate a new window docked into the previous dock space.
	// And attach a new dock space to it
	ImGui::DockBuilderDockWindow(windowStrID.c_str(), mainDockID);
	ImGui::Begin(windowStrID.c_str());

	std::string dockName = windowStrID + std::string("_DockSpace");
	ImGuiID dockspace_id = ImGui::GetID(dockName.c_str());
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);

	// Start building dock space node hierarchy
	ImGuiID dock_space_B_Left, dock_space_B_Right;
	ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.8f, &dock_space_B_Left, &dock_space_B_Right);

	ImGuiID dock_space_B_TopLeft, dock_space_B_BottomLeft;
	ImGui::DockBuilderSplitNode(dock_space_B_Left, ImGuiDir_Up, 0.7f, &dock_space_B_TopLeft, &dock_space_B_BottomLeft);

	// Attach a new dock space to it
	const char* windowName = GetWindow(W_Explorer::GetName())->GetWindowStrID();
	ImGui::DockBuilderDockWindow(windowName, dock_space_B_BottomLeft);
	((W_Explorer*)GetWindow(W_Explorer::GetName()))->LoadLayout_Default(); //W_Explorer needs an internal dock setup

	windowName = GetWindow(W_Console::GetName())->GetWindowStrID();
	ImGui::DockBuilderDockWindow(windowName, dock_space_B_BottomLeft);
	ImGui::Begin(windowName); ImGui::End(); //By flashing the window, it gets added to the node and we can change the selected tab
	ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_space_B_BottomLeft);
	node->TabBar->NextSelectedTabId = node->TabBar->Tabs.begin()->ID;

	ImGuiID leftTopLeftSpace_id, rightTopLeftSpace_id;
	ImGui::DockBuilderSplitNode(dock_space_B_TopLeft, ImGuiDir_Left, 0.2f, &leftTopLeftSpace_id, &rightTopLeftSpace_id);

	windowName = GetWindow(W_Hierarchy::GetName())->GetWindowStrID();
	ImGui::DockBuilderDockWindow(windowName, leftTopLeftSpace_id);

	ImGuiID topCenterSpace_id, bottomCenterSpace_id;
	ImGui::DockBuilderSplitNode(rightTopLeftSpace_id, ImGuiDir_Up, 0.10f, &topCenterSpace_id, &bottomCenterSpace_id);

	windowName = GetWindow(W_MainToolbar::GetName())->GetWindowStrID();
	ImGui::DockBuilderDockWindow(windowName, topCenterSpace_id);
	ImGui::DockBuilderGetNode(topCenterSpace_id)->WantHiddenTabBarToggle = true;
	
	windowName = GetWindow(W_MainViewport::GetName())->GetWindowStrID();
	ImGui::DockBuilderDockWindow(windowName, bottomCenterSpace_id);
	ImGui::DockBuilderGetNode(bottomCenterSpace_id)->WantHiddenTabBarToggle = true;
	
	ImGuiID topRightSpace_id, bottomRightSpace_id;
	ImGui::DockBuilderSplitNode(dock_space_B_Right, ImGuiDir_Up, 0.65f, &topRightSpace_id, &bottomRightSpace_id);

	windowName = GetWindow(W_Inspector::GetName())->GetWindowStrID();
	ImGui::DockBuilderDockWindow(windowName, topRightSpace_id);
	windowName = GetWindow(W_EngineConfig::GetName())->GetWindowStrID();
	ImGui::DockBuilderDockWindow(windowName, bottomRightSpace_id);
	windowName = GetWindow(W_Resources::GetName())->GetWindowStrID();
	ImGui::DockBuilderDockWindow(windowName, bottomRightSpace_id);

	ImGui::DockBuilderFinish(dockspace_id);
	ImGui::End();
}