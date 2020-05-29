#include "WindowFrame.h"

#include "Engine.h"
#include "Config.h"

#include "Window.h"
#include "M_Editor.h" //Can be removed when 'frameWindowClass' is sent in the constructor

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

WindowFrame::WindowFrame()
{

}

WindowFrame::~WindowFrame()
{
	for (uint i = 0; i < windows.size(); ++i)
		delete windows[i];

	windows.clear();
}

void WindowFrame::Draw()
{
	ImGuiWindowFlags frameWindow_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
										 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::SetNextWindowClass(Engine->moduleEditor->frameWindowClass);
	std::string displayTab = displayName + std::string("###") + name;
	ImGui::Begin(displayTab.c_str(), nullptr, frameWindow_flags);
	
	std::string dockName = name + std::string("_DockSpace");
	ImGuiID dockspace_id = ImGui::GetID(dockName.c_str());
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0, Engine->moduleEditor->normalWindowClass);

	DrawMenuBar();

	ImGui::End();

	for (uint i = 0; i < windows.size(); ++i)
	{
		if (windows[i]->IsActive())
			windows[i]->Draw();
	}
}

void WindowFrame::SaveLayout(Config& file)
{
	file.SetString("Window Name", name.c_str());

	std::string dockName = name + std::string("_DockSpace");
	ImGuiWindow* window = ImGui::FindWindowByName(name.c_str());
	ImGuiID dockspace_id = window->GetID(dockName.c_str());

	ImGuiDockNode* mainNode = ImGui::DockBuilderGetNode(dockspace_id);

	SaveDockLayout(mainNode, file.SetNode("Root Node"));
}

void WindowFrame::SaveDockLayout(ImGuiDockNode* node, Config& file)
{
	if (node->IsSplitNode())
	{
		float availableNodeSize = node->Size[node->SplitAxis] - 2.0f;

		file.SetString("Division Axis", node->SplitAxis == 0 ? "X" : "Y"); //-1 for none, 0 for x, 1 for y
		file.SetNumber("Division Value", node->ChildNodes[0]->Size[node->SplitAxis] / availableNodeSize);

		Config_Array arr = file.SetArray("Children");
		SaveDockLayout(node->ChildNodes[0], arr.AddNode());
		SaveDockLayout(node->ChildNodes[1], arr.AddNode());
	}
	else if (node->Windows.size() > 0)
	{
		file.SetString("Division Axis", "None");
		Config_Array win_arr = file.SetArray("Windows");

		for (uint i = 0; i < node->Windows.size(); ++i)
			win_arr.AddString(node->Windows[i]->Name);
	}
}

void WindowFrame::LoadLayout(Config& file)
{
	name = file.GetString("Window Name");

	//TODO: viewport should be handled differently, windows han be outside of the main viewport?
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::Begin(name.c_str());

	std::string dockName = name + std::string("_DockSpace");
	ImGuiID dockspace_id = ImGui::GetID(dockName.c_str());
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoSplit);

	LoadDockLayout(dockspace_id, file.GetNode("Root Node"));

	ImGui::End();
}

void WindowFrame::LoadDockLayout(ImGuiID dockID, Config& file)
{
	std::string divisionAxis = file.GetString("Division Axis");
	if (divisionAxis != "None")
	{
		ImGuiID childDock1, childDock2;

		float divisionValue = file.GetNumber("Division Value");
		float roundedDivision = (float)((int)(divisionValue * 100 + 0.5f)) / 100;
		ImGui::DockBuilderSplitNode(dockID, divisionAxis == "X" ? ImGuiDir_Left : ImGuiDir_Up, roundedDivision, &childDock1, &childDock2);

		Config_Array arr = file.GetArray("Children");
		LoadDockLayout(childDock1, arr.GetNode(0));
		LoadDockLayout(childDock2, arr.GetNode(1));
	}
	else
	{
		Config_Array win_arr = file.GetArray("Windows");
		for (uint i = 0; i < win_arr.GetSize(); ++i)
			ImGui::DockBuilderDockWindow(win_arr.GetString(i), dockID);
	}
}

void WindowFrame::DrawMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		MenuBar_File();
		MenuBar_Edit();
		MenuBar_Custom();
		MenuBar_Window();
		MenuBar_Help();
		MenuBar_Development();

		ImGui::EndMenuBar();
	}
}

void WindowFrame::MenuBar_File()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Save", nullptr, nullptr, false)) {}
		if (ImGui::MenuItem("Save as...", nullptr, nullptr, false)) {}
		ImGui::Separator();
		if (ImGui::MenuItem("Open Asset", nullptr, nullptr, false)) {}
		if (ImGui::MenuItem("Save All", nullptr, nullptr, false)) {}

		ImGui::EndMenu();
	}
}

void WindowFrame::MenuBar_Edit()
{
	if (ImGui::BeginMenu("Edit"))
	{
		ImGui::Text("History"); ImGui::Indent();
			if (ImGui::MenuItem("Undo", "Ctrl + Z", nullptr, false)) {}
			if (ImGui::MenuItem("Redo", "Ctrl + Y", nullptr, false)) {}
			if (ImGui::MenuItem("View Undo History", nullptr, nullptr, false)) {}
		ImGui::Unindent();  ImGui::Separator();
		ImGui::Text("Edit"); ImGui::Indent();
			if (ImGui::MenuItem("Cut", "Ctrl + X", nullptr, false)) {}
			if (ImGui::MenuItem("Copy", "Ctrl + C", nullptr, false)) {}
			if (ImGui::MenuItem("Paste", "Ctrl + V", nullptr, false)) {}
			if (ImGui::MenuItem("Duplciate", "Ctrl + W", nullptr, false)) {}
			if (ImGui::MenuItem("Delete", "Delete", nullptr, false)) {}
		ImGui::Unindent();  ImGui::Separator();
		ImGui::Text("Configuration"); ImGui::Indent();
			if (ImGui::MenuItem("Editor preferences", nullptr, nullptr, false)) {}
			if (ImGui::MenuItem("Project Settings", nullptr, nullptr, false)) {}
		ImGui::Unindent();

		ImGui::EndMenu();
	}
}

void WindowFrame::MenuBar_Window()
{
	if (ImGui::BeginMenu("Windows"))
	{
		std::vector<Window*>::iterator it;
		for (it = windows.begin(); it != windows.end(); ++it)
		{
			bool windowActive = (*it)->IsActive();
			if (ImGui::Checkbox((*it)->name.c_str(), &windowActive))
			{
				(*it)->SetActive(windowActive);
			}
		}
		ImGui::EndMenu();
	}

}

void WindowFrame::MenuBar_Help()
{
	if (ImGui::BeginMenu("Help"))
	{
		//TODO: Add "About" as a generic modal window shared by all window frames

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
}