#include "WindowFrame.h"

#include "Engine.h"
#include "Globals.h"
#include "Config.h"

#include "Window.h"
#include "M_FileSystem.h"

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
	ImGui::Begin(name.c_str(), nullptr, frameWindow_flags);
	ImGui::PopStyleVar();

	ImGuiIO& io = ImGui::GetIO();
	ImGuiID dockspace_id = 0;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		std::string dockName = name + std::string("_DockSpace");
		dockspace_id = ImGui::GetID(dockName.c_str());
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
	}

	DrawMenuBar();
	ImGui::End();

	for (uint i = 0; i < windows.size(); ++i)
	{
		if (windows[i]->IsActive())
			windows[i]->Draw();
	}
/*
	Config layoutFile;
	SaveLayout(layoutFile);

	char* buffer = nullptr;
	uint size = layoutFile.Serialize(&buffer);
	if (size > 0)
	{
		Engine->fileSystem->Save("Engine/Layouts/Default.lay", buffer, size);
		RELEASE_ARRAY(buffer);
	}
*/
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
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);

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

void WindowFrame::LoadLayout_ForceDefault(Config& file)
{
	//TODO: viewport should be handled differently, windows han be outside of the main viewport?
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::Begin(name.c_str());

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
	ImGui::DockBuilderDockWindow("Scene", bottomCenterSpace_id);

	ImGuiID topRightSpace_id, bottomRightSpace_id;
	ImGui::DockBuilderSplitNode(rightspace_id, ImGuiDir_Up, 0.65f, &topRightSpace_id, &bottomRightSpace_id);

	ImGui::DockBuilderDockWindow("Inspector", topRightSpace_id);
	ImGui::DockBuilderDockWindow("Engine Config", bottomRightSpace_id);
	ImGui::DockBuilderDockWindow("Resources", bottomRightSpace_id);

	ImGui::DockBuilderFinish(dockspace_id);
	ImGui::End();
}
