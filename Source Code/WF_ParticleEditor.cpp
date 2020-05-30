#include "WF_ParticleEditor.h"

#include "W_Emitters.h"
#include "W_ParticleDetails.h"
#include "W_ParticleCurves.h"
#include "W_ParticleViewport.h"
#include "W_ParticleToolbar.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

WF_ParticleEditor::WF_ParticleEditor(M_Editor* editor, ImGuiWindowClass* frameWindowClass, ImGuiWindowClass* windowClass, int ID) : WindowFrame(GetName(), frameWindowClass, windowClass, ID)
{
	windows.push_back(new W_Emitters(editor, windowClass, ID, this));
	windows.push_back(new W_ParticleDetails(editor, windowClass, ID, this));
	windows.push_back(new W_ParticleCurves(editor, windowClass, ID, this));
	windows.push_back(new W_ParticleViewport(editor, windowClass, ID, this));
	windows.push_back(new W_ParticleToolbar(editor, windowClass, ID, this));
}

WF_ParticleEditor::~WF_ParticleEditor()
{

}

void WF_ParticleEditor::SetParticleSystem(R_ParticleSystem* particleSystem)
{
	this->particleSystem = particleSystem;
}

void WF_ParticleEditor::MenuBar_Custom()
{
	if (ImGui::BeginMenu("Custom"))
	{
		ImGui::EndMenu();
	}
}

void WF_ParticleEditor::MenuBar_Development()
{
	if (ImGui::BeginMenu("Development"))
	{
		ImGui::EndMenu();
	}
}

void WF_ParticleEditor::LoadLayout_ForceDefault(Config& file, ImGuiID mainDockID)
{
	std::string windowNameID = std::string("###").append(name);
	ImGui::DockBuilderDockWindow(windowNameID.c_str(), mainDockID);
	ImGui::Begin(windowNameID.c_str());

	std::string dockName = name + std::string("_DockSpace");
	ImGuiID dockspace_id = ImGui::GetID(dockName.c_str());
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);
	/*
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
	*/
	ImGui::DockBuilderFinish(dockspace_id);
	ImGui::End();
}