#include "WF_ParticleEditor.h"

#include "Engine.h"
#include "M_SceneManager.h"
#include "Scene.h"
#include "GameObject.h"

#include "W_Emitters.h"
#include "W_ParticleDetails.h"
#include "W_ParticleCurves.h"
#include "W_ParticleViewport.h"
#include "W_ParticleToolbar.h"

#include "C_ParticleSystem.h"
#include "R_ParticleSystem.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

WF_ParticleEditor::WF_ParticleEditor(M_Editor* editor, ImGuiWindowClass* frameWindowClass, ImGuiWindowClass* windowClass, int ID) : WindowFrame(GetName(), frameWindowClass, windowClass, ID)
{
	windows.push_back(new W_Emitters(this, windowClass, ID));
	windows.push_back(new W_ParticleDetails(this, windowClass, ID));
	windows.push_back(new W_ParticleCurves(this, windowClass, ID));
	windows.push_back(new W_ParticleViewport(this, windowClass, ID));
	windows.push_back(new W_ParticleToolbar(this, windowClass, ID));
}

WF_ParticleEditor::~WF_ParticleEditor()
{
	Engine->sceneManager->RemoveScene(scene);
}

void WF_ParticleEditor::SetResource(uint64 resourceID)
{
	WindowFrame::SetResource(resourceID);
	particleSystem = (R_ParticleSystem*)resourceHandle.Get();

	scene = Engine->sceneManager->CreateNewScene();
	GameObject* gameObject = scene->CreateNewGameObject("Particle System");
	C_ParticleSystem*  particleComponent = (C_ParticleSystem*)gameObject->CreateComponent(Component::ParticleSystem);
	particleComponent->SetResource(resourceID);

	W_ParticleViewport* viewport = (W_ParticleViewport*)GetWindow(W_ParticleViewport::GetName());
	viewport->SetScene(scene);
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

void WF_ParticleEditor::LoadLayout_Default(ImGuiID mainDockID)
{
	ImGui::DockBuilderDockWindow(windowStrID.c_str(), mainDockID);
	ImGui::Begin(windowStrID.c_str());

	std::string dockName = windowStrID + std::string("_DockSpace");
	ImGuiID dockspace_id = ImGui::GetID(dockName.c_str());
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);
	
	ImGuiID topSpace_id, bottomSpace_id;
	ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.08f, &topSpace_id, &bottomSpace_id);
	
	const char* windowName = GetWindow(W_ParticleToolbar::GetName())->GetWindowStrID();
	ImGui::DockBuilderDockWindow(windowName, topSpace_id);

	ImGuiID leftBottomSpace_id, rightBottomSpace_id;
	ImGui::DockBuilderSplitNode(bottomSpace_id, ImGuiDir_Left, 0.24f, &leftBottomSpace_id, &rightBottomSpace_id);

	ImGuiID upLeftBottomSpace_id, bottomLeftBottomSpace_id;
	ImGui::DockBuilderSplitNode(leftBottomSpace_id, ImGuiDir_Up, 0.54f, &upLeftBottomSpace_id, &bottomLeftBottomSpace_id);

	windowName = GetWindow(W_ParticleViewport::GetName())->GetWindowStrID();
	ImGui::DockBuilderDockWindow(windowName, upLeftBottomSpace_id);

	windowName = GetWindow(W_ParticleDetails::GetName())->GetWindowStrID();
	ImGui::DockBuilderDockWindow(windowName, bottomLeftBottomSpace_id);

	ImGuiID upRightBottomSpace_id, bottomRightBottomSpace_id;
	ImGui::DockBuilderSplitNode(rightBottomSpace_id, ImGuiDir_Up, 0.7f, &upRightBottomSpace_id, &bottomRightBottomSpace_id);

	windowName = GetWindow(W_Emitters::GetName())->GetWindowStrID();
	ImGui::DockBuilderDockWindow(windowName, upRightBottomSpace_id);
	windowName = GetWindow(W_ParticleCurves::GetName())->GetWindowStrID();
	ImGui::DockBuilderDockWindow(windowName, bottomRightBottomSpace_id);
	
	ImGui::DockBuilderFinish(dockspace_id);
	ImGui::End();
}