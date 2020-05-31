#include "WF_ParticleEditor.h"

#include "W_Emitters.h"
#include "W_ParticleDetails.h"
#include "W_ParticleCurves.h"
#include "W_ParticleViewport.h"
#include "W_ParticleToolbar.h"

#include "R_ParticleSystem.h"

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
	displayName = particleSystem->GetName();
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
	std::string windowStrID = displayName + std::string("###") + name + ("_") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowStrID.c_str(), mainDockID);
	ImGui::Begin(windowStrID.c_str());

	std::string dockName = windowStrID + std::string("_DockSpace");
	ImGuiID dockspace_id = ImGui::GetID(dockName.c_str());
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);
	
	ImGuiID topSpace_id, bottomSpace_id;
	ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.08f, &topSpace_id, &bottomSpace_id);
	
	std::string windowName = std::string("Toolbar") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), topSpace_id);

	ImGuiID leftBottomSpace_id, rightBottomSpace_id;
	ImGui::DockBuilderSplitNode(bottomSpace_id, ImGuiDir_Left, 0.24f, &leftBottomSpace_id, &rightBottomSpace_id);

	ImGuiID upLeftBottomSpace_id, bottomLeftBottomSpace_id;
	ImGui::DockBuilderSplitNode(leftBottomSpace_id, ImGuiDir_Up, 0.54f, &upLeftBottomSpace_id, &bottomLeftBottomSpace_id);

	windowName = std::string("Viewport") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), upLeftBottomSpace_id);

	windowName = std::string("Details") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), bottomLeftBottomSpace_id);

	ImGuiID upRightBottomSpace_id, bottomRightBottomSpace_id;
	ImGui::DockBuilderSplitNode(rightBottomSpace_id, ImGuiDir_Up, 0.7f, &upRightBottomSpace_id, &bottomRightBottomSpace_id);

	windowName = std::string("Emitters") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), upRightBottomSpace_id);
	windowName = std::string("Curve Editor") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), bottomRightBottomSpace_id);
	
	ImGui::DockBuilderFinish(dockspace_id);
	ImGui::End();
}