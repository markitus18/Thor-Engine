#include "WF_SceneEditor.h"

#include "Engine.h"
#include "Globals.h"

#include "M_Scene.h"
#include "M_Editor.h"
#include "M_Camera3D.h"
#include "M_Resources.h"
#include "M_FileSystem.h"

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

#include "R_Scene.h"

WF_SceneEditor::WF_SceneEditor(M_Editor* editor, ImGuiWindowClass* frameWindowClass, ImGuiWindowClass* windowClass, int ID) : WindowFrame(GetName(), frameWindowClass, windowClass, ID)
{
	isDockable = false;

	explorerWindowClass = new ImGuiWindowClass();
	explorerWindowClass->ClassId = 3;

	windows.push_back(new W_Hierarchy(editor, windowClass, ID));
	windows.push_back(new W_Scene(editor, windowClass, ID));
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
	displayName = Engine->moduleResources->GetResource(resourceID)->name; 
}

void WF_SceneEditor::MenuBar_File()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New Scene"))
		{
			Engine->moduleEditor->LoadScene("Engine/Assets/Defaults/Untitled.scene");
		}

		if (ImGui::BeginMenu("Open Scene"))
		{
			//FIXME: avoid doing this every frame
			std::vector<std::string> sceneList;
			sceneList.clear();
			Engine->fileSystem->GetAllFilesWithExtension("", "scene", sceneList);

			for (uint i = 0; i < sceneList.size(); i++)
			{
				if (ImGui::MenuItem(sceneList[i].c_str()))
				{
					Engine->scene->LoadScene(sceneList[i].c_str());
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Save Scene"))
		{
			if (Engine->scene->scene->GetOriginalFile() == "Engine/Assets/Defaults/Untitled.scene")
			{
				Engine->moduleEditor->OpenFileNameWindow();
			}
			else
			{
				Engine->moduleResources->SaveResource((Resource*)Engine->scene->scene);
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
			std::string name(Engine->scene->GetNewGameObjectName("GameObject"));
			GameObject* newGameObject = Engine->scene->CreateGameObject(name.c_str());
			Engine->moduleEditor->SelectSingle((TreeNode*)newGameObject);
		}

		if (ImGui::MenuItem("Empty Child"))
		{
			GameObject* parent = (GameObject*)(Engine->moduleEditor->selectedGameObjects.size() > 0 ? Engine->moduleEditor->selectedGameObjects[0] : nullptr);
			std::string name(Engine->scene->GetNewGameObjectName("GameObject", parent));
			GameObject* newGameObject = Engine->scene->CreateGameObject(name.c_str(), parent);
			Engine->moduleEditor->SelectSingle((TreeNode*)newGameObject);
		}

		if (ImGui::MenuItem("Empty Child x10"))
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
		if (ImGui::MenuItem("Camera"))
		{
			Engine->scene->CreateCamera();
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
			ImGui::MenuItem("Quadtree", nullptr, &Engine->scene->drawQuadtree);
			ImGui::MenuItem("Ray picking", nullptr, &Engine->camera->drawRay);
			ImGui::MenuItem("GameObjects box", nullptr, &Engine->scene->drawBounds);
			ImGui::MenuItem("GameObjects box (selected)", nullptr, &Engine->scene->drawBoundsSelected);
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
}

void WF_SceneEditor::LoadLayout_Default(ImGuiID mainDockID)
{
	// Generate a new window docked into the previous dock space.
	// And attach a new dock space to it
	std::string windowStrID = displayName + std::string("###") + name + ("_") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowStrID.c_str(), mainDockID);
	ImGui::Begin(windowStrID.c_str());

	std::string dockName = windowStrID + std::string("_DockSpace");
	ImGuiID dock_space_B = ImGui::GetID(dockName.c_str());
	ImGui::DockSpace(dock_space_B, ImVec2(0.0f, 0.0f), 0);

	// Start building dock space node hierarchy
	ImGuiID dock_space_B_Left, dock_space_B_Right;
	ImGui::DockBuilderSplitNode(dock_space_B, ImGuiDir_Left, 0.8f, &dock_space_B_Left, &dock_space_B_Right);

	ImGuiID dock_space_B_TopLeft, dock_space_B_BottomLeft;
	ImGui::DockBuilderSplitNode(dock_space_B_Left, ImGuiDir_Up, 0.7f, &dock_space_B_TopLeft, &dock_space_B_BottomLeft);

	// Dock new window to one of the newly generated docks (the issue begins here)
	// Attach a new dock space to it
	std::string windowName = std::string("Explorer") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), dock_space_B_BottomLeft);
	ImGui::Begin(windowName.c_str());
	
	dockName = windowName + "_DockSpace";
	ImGuiID dock_space_C = ImGui::GetID(dockName.c_str());
	ImGui::DockSpace(dock_space_C, ImVec2(0.0f, 0.0f), 0, explorerWindowClass);

	// Split the new dock in two pieces (top and bottom)
	ImGuiID dock_space_C_Top, dock_space_C_Bottom;
	ImGui::DockBuilderSplitNode(dock_space_C, ImGuiDir_Up, 0.10f, &dock_space_C_Top, &dock_space_C_Bottom);

	windowName = std::string("Explorer_Toolbar") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), dock_space_C_Top);
	ImGui::DockBuilderGetNode(dock_space_C_Top)->WantHiddenTabBarToggle = true;
	//ImGui::DockBuilderGetNode(dock_space_C_Top)->LocalFlags |= ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_AutoHideTabBar;

	// Split the bottom dock in two more pieces (left and right)
	ImGuiID leftExplorerSpace_id, rightExplorerSpace_id;
	ImGui::DockBuilderSplitNode(dock_space_C_Bottom, ImGuiDir_Left, 0.15f, &leftExplorerSpace_id, &rightExplorerSpace_id);

	// Attach windows to the different nodes (code irrelevant from this point)
	windowName = std::string("Explorer_Tree") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), leftExplorerSpace_id);
	ImGui::DockBuilderGetNode(leftExplorerSpace_id)->LocalFlags |= ImGuiDockNodeFlags_AutoHideTabBar;

	windowName = std::string("Explorer_Folder") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), rightExplorerSpace_id);
	ImGui::DockBuilderGetNode(leftExplorerSpace_id)->LocalFlags |= ImGuiDockNodeFlags_AutoHideTabBar;
	
	ImGui::End();
	//------------------------------------------------------------------------------------------

	windowName = std::string("Console") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), dock_space_B_BottomLeft);

	ImGuiID leftTopLeftSpace_id, rightTopLeftSpace_id;
	ImGui::DockBuilderSplitNode(dock_space_B_TopLeft, ImGuiDir_Left, 0.2f, &leftTopLeftSpace_id, &rightTopLeftSpace_id);

	windowName = std::string("Hierarchy") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), leftTopLeftSpace_id);

	ImGuiID topCenterSpace_id, bottomCenterSpace_id;
	ImGui::DockBuilderSplitNode(rightTopLeftSpace_id, ImGuiDir_Up, 0.10f, &topCenterSpace_id, &bottomCenterSpace_id);

	windowName = std::string("Toolbar") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), topCenterSpace_id);
	ImGuiDockNode* node = ImGui::DockBuilderGetNode(topCenterSpace_id);
	node->WantHiddenTabBarToggle = true;

	windowName = std::string("Scene") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), bottomCenterSpace_id);
	ImGui::DockBuilderGetNode(bottomCenterSpace_id)->WantHiddenTabBarToggle = true;

	ImGuiID topRightSpace_id, bottomRightSpace_id;
	ImGui::DockBuilderSplitNode(dock_space_B_Right, ImGuiDir_Up, 0.65f, &topRightSpace_id, &bottomRightSpace_id);

	windowName = std::string("Inspector") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), topRightSpace_id);
	windowName = std::string("Engine Config") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), bottomRightSpace_id);
	windowName = std::string("Resources") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), bottomRightSpace_id);

	ImGui::End();
}