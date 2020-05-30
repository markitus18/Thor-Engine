#include "WF_SceneEditor.h"

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

WF_SceneEditor::WF_SceneEditor(M_Editor* editor, ImGuiWindowClass* frameWindowClass, ImGuiWindowClass* windowClass, int ID) : WindowFrame(GetName(), frameWindowClass, windowClass, ID)
{
	isDockable = false;

	windows.push_back(new W_Hierarchy(editor, windowClass, ID));
	windows.push_back(new W_Scene(editor, windowClass, ID));
	windows.push_back(new W_Inspector(editor, windowClass, ID));
	windows.push_back(new W_Explorer(editor, windowClass, ID));
	windows.push_back(new W_Console(editor, windowClass, ID));
	windows.push_back(new W_Resources(editor, windowClass, ID));
	windows.push_back(new W_EngineConfig(editor, windowClass, ID));
	windows.push_back(new W_MainToolbar(editor, windowClass, ID));

	W_About* w_about = new W_About(editor, windowClass, ID);
	w_about->SetActive(false);
	windows.push_back(w_about);
}

WF_SceneEditor::~WF_SceneEditor()
{

}

void WF_SceneEditor::OnSceneChange(const char* newSceneFile)
{
	std::string sceneName = "";
	Engine->fileSystem->SplitFilePath(newSceneFile, nullptr, &sceneName);
	displayName = sceneName + std::string(".scene");
}

void WF_SceneEditor::MenuBar_File()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New Scene"))
		{
			Engine->scene->CreateDefaultScene();
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

void WF_SceneEditor::LoadLayout_ForceDefault(Config& file, ImGuiID mainDockID)
{
	std::string windowNameID = std::string("###").append(name);
	ImGui::DockBuilderDockWindow(windowNameID.c_str(), mainDockID);
	ImGui::Begin(windowNameID.c_str());

	std::string dockName = name + std::string("_DockSpace");
	ImGuiID dockspace_id = ImGui::GetID(dockName.c_str());
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);

	ImGuiID leftSpace_id, rightspace_id;
	ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.8f, &leftSpace_id, &rightspace_id);

	ImGuiID topLeftSpace_id, bottomLeftSpace_id;
	ImGui::DockBuilderSplitNode(leftSpace_id, ImGuiDir_Up, 0.7f, &topLeftSpace_id, &bottomLeftSpace_id);

	ImGui::DockBuilderDockWindow("Explorer", bottomLeftSpace_id);
	ImGui::DockBuilderDockWindow("Console", bottomLeftSpace_id);

	ImGuiID leftTopLeftSpace_id, rightTopLeftSpace_id;
	ImGui::DockBuilderSplitNode(topLeftSpace_id, ImGuiDir_Left, 0.2f, &leftTopLeftSpace_id, &rightTopLeftSpace_id);

	ImGui::DockBuilderDockWindow("Hierarchy", leftTopLeftSpace_id);

	ImGuiID topCenterSpace_id, bottomCenterSpace_id;
	ImGui::DockBuilderSplitNode(rightTopLeftSpace_id, ImGuiDir_Up, 0.10f, &topCenterSpace_id, &bottomCenterSpace_id);

	ImGui::DockBuilderDockWindow("Toolbar", topCenterSpace_id);
	ImGuiDockNode* node = ImGui::DockBuilderGetNode(topCenterSpace_id);
	node->WantHiddenTabBarToggle = true;
	ImGui::DockBuilderDockWindow("Scene", bottomCenterSpace_id);
	node = ImGui::DockBuilderGetNode(bottomCenterSpace_id);
	node->WantHiddenTabBarToggle = true;

	ImGuiID topRightSpace_id, bottomRightSpace_id;
	ImGui::DockBuilderSplitNode(rightspace_id, ImGuiDir_Up, 0.65f, &topRightSpace_id, &bottomRightSpace_id);

	ImGui::DockBuilderDockWindow("Inspector", topRightSpace_id);
	ImGui::DockBuilderDockWindow("Engine Config", bottomRightSpace_id);
	ImGui::DockBuilderDockWindow("Resources", bottomRightSpace_id);

	ImGui::DockBuilderFinish(dockspace_id);
	ImGui::End();
}