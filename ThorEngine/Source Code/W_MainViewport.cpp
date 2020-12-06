#include "W_MainViewport.h"

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

#include "R_Texture.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui_internal.h"

W_MainViewport::W_MainViewport(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID) : WViewport(parent, GetName(), windowClass, ID)
{
	statsMask = EViewportStats::FPS | EViewportStats::MeshCount | EViewportStats::TriangleCount | EViewportStats::ObjectCount;
	statsDisplayed = statsMask; //Temporal, to display all stats at the start
	ImGuizmo::Enable(true);

	// Toolbar icons
	hTranslateIcon.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/TranslateIcon.png")->ID);
	hRotateIcon.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/RotateIcon.png")->ID);
	hScaleIcon.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/ScaleIcon.png")->ID);
}

void W_MainViewport::DrawToolbarCustom()
{
	ImVec2 cursorPos = ImGui::GetCurrentWindow()->DC.CursorStartPos + ImGui::GetStyle().WindowPadding;
	cursorPos += ImVec2(ImGui::GetWindowSize().x - 400.0f, 0.0f);
	ImGui::SetCursorScreenPos(cursorPos);

	float toolbarHeight = 23.0f;
	ImVec2 imageSize = ImVec2(toolbarHeight, toolbarHeight) - ImGui::GetStyle().FramePadding * 2;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 2.0f));
	ImVec4 highlight = gizmoOperation == ImGuizmo::OPERATION::TRANSLATE ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	if (ImGui::ImageButton((ImTextureID)hTranslateIcon.Get()->buffer, imageSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), -1, ImVec4(), highlight))
	{
		gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	}
	
	ImGui::SameLine();
	highlight = gizmoOperation == ImGuizmo::OPERATION::ROTATE ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	if (ImGui::ImageButton((ImTextureID)hRotateIcon.Get()->buffer, imageSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), -1, ImVec4(), highlight))
	{
		gizmoOperation = ImGuizmo::OPERATION::ROTATE;
	}

	ImGui::SameLine();
	highlight = gizmoOperation == ImGuizmo::OPERATION::SCALE ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	if (ImGui::ImageButton((ImTextureID)hScaleIcon.Get()->buffer, imageSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), -1, ImVec4(), highlight))
	{
		gizmoOperation = ImGuizmo::OPERATION::SCALE;
	}
	ImGui::PopStyleVar();

	WViewport::DrawToolbarCustom();
}