#include "W_Hierarchy.h"

#include "Engine.h"
#include "M_Scene.h"
#include "M_Editor.h"

#include "GameObject.h"

#include "ImGui/imgui.h"

W_Hierarchy::W_Hierarchy(M_Editor* editor) : Window(editor, "Hierarchy")
{

}

void W_Hierarchy::Draw()
{
	ImGui::SetNextWindowClass(editor->normalWindowClass);
	if (ImGui::Begin("Hierarchy", &active))
		DrawTree(Engine->scene->GetRoot());

	ImGui::End();
}