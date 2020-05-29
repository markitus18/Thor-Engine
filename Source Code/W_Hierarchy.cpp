#include "W_Hierarchy.h"

#include "Engine.h"
#include "M_Scene.h"

#include "GameObject.h"

#include "ImGui/imgui.h"

W_Hierarchy::W_Hierarchy(M_Editor* editor, ImGuiWindowClass* windowClass) : Window(editor, GetName(), windowClass)
{

}

void W_Hierarchy::Draw()
{
	ImGui::SetNextWindowClass(windowClass);
	if (ImGui::Begin("Hierarchy", &active))
		DrawTree(Engine->scene->GetRoot());

	ImGui::End();
}