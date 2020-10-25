#include "WindowFrame.h"

#include "Engine.h"
#include "Config.h"

#include "M_Resources.h"

#include "Window.h"
#include "Resource.h"

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
	if (pendingLoadLayout) return;

	ImGuiWindowFlags frameWindow_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	if (!isDockable)
		frameWindow_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

	//Window settings and display
	ImGui::SetNextWindowClass(frameWindowClass);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin(windowStrID.c_str(), isDockable ? &active : nullptr, frameWindow_flags);
	ImGui::PopStyleVar();

	//Adding a dock space to dock all child windows
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

void WindowFrame::SetResource(uint64 resourceID)
{
	resourceHandle.Set(resourceID);
	displayName = resourceHandle.Get()->GetName();
	windowStrID = displayName + std::string("###") + name + ("_") + std::to_string(ID);
}

Window* WindowFrame::GetWindow(const char* name) const
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
			if (ImGui::MenuItem("Duplicate", "Ctrl + W", nullptr, false)) {}
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
		if (ImGui::MenuItem("Save Layout", nullptr, nullptr, false)){}
		if (ImGui::MenuItem("Load Layout", nullptr, nullptr, false)){}
		if (ImGui::MenuItem("Reset Layout", nullptr, nullptr, false)) {}
		ImGui::Separator();

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
