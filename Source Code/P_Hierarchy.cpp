#include "P_Hierarchy.h"
#include "Application.h"
#include "M_Scene.h"
#include "GameObject.h"
#include "M_Camera3D.h"
#include "C_Camera.h"
#include "C_Transform.h"
#include "C_Mesh.h"	
#include "C_Material.h"

#include "ImGui\imgui.h"

#include "Application.h"
#include "M_Editor.h"
#include "M_Input.h"

#include "TreeNode.h"

P_Hierarchy::P_Hierarchy()
{
}

P_Hierarchy::~P_Hierarchy()
{

}

void P_Hierarchy::Draw(ImGuiWindowFlags flags)
{
	if (active)
	{
		ImGui::SetNextWindowPos(position);
		ImGui::SetNextWindowSize(size);

		ImGui::Begin("Hierarchy", &active, flags);
		
		if (ImGui::IsMouseHoveringWindow())
		{
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
				ImGui::SetWindowFocus();
		}


		DrawTree(App->scene->GetRoot());
		ImGui::End();
	}
}

void P_Hierarchy::UpdatePosition(int screen_width, int screen_height)
{
	position.x = 0;
	position.y = 19;
	size.x = screen_width * (0.20);
	size.y = screen_height * (0.60) - position.y;

}