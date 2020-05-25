#include "W_Hierarchy.h"

#include "Engine.h"
#include "M_Scene.h"

#include "GameObject.h"

#include "ImGui/imgui.h"

W_Hierarchy::W_Hierarchy(M_Editor* editor) : DWindow(editor, "Hierarchy")
{

}

void W_Hierarchy::Draw()
{
	if (ImGui::Begin("Hierarchy"))
	{
		DrawTree(Engine->scene->GetRoot());
		ImGui::End();
	}
}

void W_Hierarchy::OnResize()
{

}