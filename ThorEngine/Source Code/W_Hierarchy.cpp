#include "W_Hierarchy.h"

#include "Engine.h"
#include "M_SceneManager.h"

#include "GameObject.h"

#include "ImGui/imgui.h"

W_Hierarchy::W_Hierarchy(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID) : Window(parent, GetName(), windowClass, ID)
{

}

void W_Hierarchy::Draw()
{
	ImGui::SetNextWindowClass(windowClass);
	if (ImGui::Begin(windowStrID.c_str(), &active))
		DrawTree(Engine->sceneManager->GetRoot());

	ImGui::End();
}