#include "WindowFrame.h"

#include "Engine.h"
#include "Config.h"

#include "Window.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

WindowFrame::WindowFrame(const char* name, ImGuiWindowClass* frameWindowClass, ImGuiWindowClass* windowClass, int ID) : name(name), displayName(name),
						frameWindowClass(frameWindowClass), windowClass(windowClass), ID(ID)
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
	ImGuiWindowFlags frameWindow_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	if (!isDockable)
		frameWindow_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowClass(frameWindowClass);
	std::string windowStrID = displayName + std::string("###") + name + ("_") + std::to_string(ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin(windowStrID.c_str(), nullptr, frameWindow_flags);
	ImGui::PopStyleVar();

	std::string dockName = windowStrID + std::string("_DockSpace");
	ImGuiID dockspace_id = ImGui::GetID(dockName.c_str());
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0, windowClass);

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
	std::string windowStrID = displayName + std::string("###") + name + ("_") + std::to_string(ID);
	std::string dockName = windowStrID + std::string("_DockSpace");

	ImGuiWindow* window = ImGui::FindWindowByName(windowStrID.c_str());
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
	else
	{
		file.SetString("Division Axis", "None");
		Config_Array win_arr = file.SetArray("Windows");

		for (uint i = 0; i < node->Windows.size(); ++i)
		{
			Config win = win_arr.AddNode();
			win.SetString("Window Name", node->Windows[i]->Name);
			win.SetBool("Hidden Tab", node->IsHiddenTabBar());
			win.SetBool("Window Visible", node->Windows[i] == node->VisibleWindow);
		}
	}
}

void WindowFrame::LoadLayout(Config& file, ImGuiID mainDockID)
{
	std::string windowStrID = displayName + std::string("###") + name + ("_") + std::to_string(ID);
 	ImGui::DockBuilderDockWindow(windowStrID.c_str(), mainDockID);
	ImGui::Begin(windowStrID.c_str());


	std::string dockName = windowStrID + std::string("_DockSpace");
	ImGuiID dockspace_id = ImGui::GetID(dockName.c_str());
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoSplit);

	LoadDockLayout(dockspace_id, file.GetNode("Root Node"));

	ImGui::End();
}

void WindowFrame::LoadDockLayout(ImGuiID dockID, Config& file)
{
	std::string divisionAxis = file.GetString("Division Axis");
	//Node has child nodes. Divide it and load each child.
	if (divisionAxis != "None")
	{
		ImGuiID childDock1, childDock2;

		float divisionValue = file.GetNumber("Division Value");
		ImGui::DockBuilderSplitNode(dockID, divisionAxis == "X" ? ImGuiDir_Left : ImGuiDir_Up, divisionValue, &childDock1, &childDock2);

		Config_Array arr = file.GetArray("Children");
		LoadDockLayout(childDock1, arr.GetNode(0));
		LoadDockLayout(childDock2, arr.GetNode(1));
	}
	//Node contains at least one window. Empty nodes are not left out in the current system
	else
	{
		Config_Array win_arr = file.GetArray("Windows");
		for (uint i = 0; i < win_arr.GetSize(); ++i)
		{
			Config win = win_arr.GetNode(i);
			std::string windowName = win.GetString("Window Name") + ("##") + std::to_string(ID);
			ImGui::DockBuilderDockWindow(windowName.c_str(), dockID);
		
			if (win.GetBool("Hidden Tab"))
			{
				ImGuiDockNode* dockNode = ImGui::DockBuilderGetNode(dockID);
				dockNode->WantHiddenTabBarToggle = true;
			}
			else if (win.GetBool("Window Visible"))
			{
				//TODO: Current DockBuilder system doesn't support loading the last active window
				ImGuiDockNode* dockNode = ImGui::DockBuilderGetNode(dockID);
				//dockNode->TabBar->SelectedTabId = ImGui::FindOrCreateWindowSettings(windowName.c_str())->ID;
				//dockNode->SelectedTabId = ImGui::FindOrCreateWindowSettings(windowName.c_str())->ID;
			}
		}
	}
}

Window* WindowFrame::GetWindow(const char* name)
{
	for (uint i = 0; i < windows.size(); ++i)
		if (windows[i]->name == name) return windows[i];

	return nullptr;
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
	if (ImGui::BeginMenu("Window"))
	{
		ImGui::Text("Display"); ImGui::Indent();
		std::vector<Window*>::iterator it;
		for (it = windows.begin(); it != windows.end(); ++it)
		{
			bool windowActive = (*it)->IsActive();
			if (ImGui::MenuItem((*it)->name.c_str(), nullptr, &windowActive))
			{
				(*it)->SetActive(windowActive);
			}
		}
		ImGui::Unindent(); ImGui::Separator();
		ImGui::Text("Layout"); ImGui::Indent();
		if (ImGui::MenuItem("Save Layout"))
		{
			//TODO: Open modal window to display layout name input / select from existing one
			requestLayoutSave = true;
		}
		if (ImGui::MenuItem("Load Layout", nullptr, nullptr, false))
		{
			//TODO: Open modal window to select existing layout list
			requestLayoutLoad = true;
		}
		if (ImGui::MenuItem("Reset Layout"))
		{
			requestLayoutLoad = true;
		}
		ImGui::Unindent();
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