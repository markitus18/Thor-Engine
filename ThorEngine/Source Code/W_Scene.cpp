#include "W_Scene.h"

#include "ImGui\imgui.h"
#include "Glew/include/glew.h"

#include "Engine.h"
#include "M_Window.h"
#include "M_Renderer3D.h"
#include "M_Input.h"
#include "M_Editor.h"
#include "M_Resources.h"
#include "M_SceneManager.h"

#include "C_Camera.h"
#include "TreeNode.h"
#include "GameObject.h"
#include "C_Transform.h"
#include "Component.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui_internal.h"

W_MainViewport::W_MainViewport(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID) : WViewport(parent, GetName(), windowClass, ID)
{
	ImGuizmo::Enable(true);
}

void W_MainViewport::DrawToolbarCustom()
{

}