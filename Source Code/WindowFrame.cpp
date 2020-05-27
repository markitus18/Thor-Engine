#include "WindowFrame.h"

#include "Engine.h"
#include "Globals.h"
#include "Config.h"

#include "Window.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

WindowFrame::WindowFrame()
{

}

WindowFrame::~WindowFrame()
{

}

void WindowFrame::Draw()
{
	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags frameWindow_flags =
	ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking| ImGuiWindowFlags_NoTitleBar |
	ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
	ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", nullptr, frameWindow_flags);
	ImGui::PopStyleVar();

	ImGuiIO& io = ImGui::GetIO();
	ImGuiID dockspace_id = 0;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		std::string dockName = name + std::string("_DockSpace");
		dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
	}

	DrawMenuBar();
	ImGui::End();

	for (uint i = 0; i < windows.size(); ++i)
	{
		if (windows[i]->IsActive())
			windows[i]->Draw();
	}
}

void WindowFrame::LoadLayout(Config& file)
{
	//TODO: viewport should be handled differently, windows han be outside of the main viewport?
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::Begin("DockSpace Demo");

	std::string dockName = name + std::string("_DockSpace");
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
}

void WindowFrame::SaveLayout(Config& file)
{

}