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
#include "ImGuiHelper.h"

W_MainViewport::W_MainViewport(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID) : WViewport(parent, GetName(), windowClass, ID)
{
	statsMask = EViewportStats::FPS | EViewportStats::MeshCount | EViewportStats::TriangleCount | EViewportStats::ObjectCount;
	statsDisplayed = statsMask; //Temporal, to display all stats at the start
	ImGuizmo::Enable(true);

	// Toolbar icons
	hTranslateIcon.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/TranslateIcon.png")->ID);
	hRotateIcon.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/RotateIcon.png")->ID);
	hScaleIcon.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/ScaleIcon.png")->ID);

	hLocalGizmoIcon.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/LocalAxisIcon.png")->ID);
	hWorldGizmoIcon.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/WorldAxisIcon.png")->ID);

	hGridSnapIcon.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/GridIcon.png")->ID);
	hRotationSnapIcon.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/RotationSnapIcon.png")->ID);
	hScaleSnapIcon.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/ScaleSnapIcon.png")->ID);

}

void W_MainViewport::DrawToolbarCustom()
{
	ImVec2 cursorPos = ImGui::GetCurrentWindow()->DC.CursorStartPos + ImGui::GetStyle().WindowPadding;
	cursorPos += ImVec2(ImGui::GetWindowSize().x - 400.0f, 0.0f);
	ImGui::SetCursorScreenPos(cursorPos);

	float toolbarHeight = 26.0f;
	ImVec2 imageSize = ImVec2(toolbarHeight, toolbarHeight) - ImGui::GetStyle().FramePadding * 2;

	// Translate operation
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 2.0f));
	if (ImGuiHelper::HighlightedImageButton((ImTextureID)hTranslateIcon.Get()->buffer, imageSize, gizmoOperation == ImGuizmo::OPERATION::TRANSLATE))
	{
		gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	}
	
	// Rotate operation
	ImGui::SameLine();
	if (ImGuiHelper::HighlightedImageButton((ImTextureID)hRotateIcon.Get()->buffer, imageSize, gizmoOperation == ImGuizmo::OPERATION::ROTATE))
	{
		gizmoOperation = ImGuizmo::OPERATION::ROTATE;
	}

	// Scale operation
	ImGui::SameLine();
	if (ImGuiHelper::HighlightedImageButton((ImTextureID)hScaleIcon.Get()->buffer, imageSize, gizmoOperation == ImGuizmo::OPERATION::SCALE))
	{
		gizmoOperation = ImGuizmo::OPERATION::SCALE;
	}

	// Gizmo mode: World vs. Local
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));	ImGui::SameLine();
	uint textureBuffer = (gizmoOperation == ImGuizmo::OPERATION::SCALE || gizmoMode == ImGuizmo::MODE::LOCAL) ? hLocalGizmoIcon.Get()->buffer : hWorldGizmoIcon.Get()->buffer;
	if (ImGui::ImageButton((ImTextureID)textureBuffer, imageSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)))
	{
		gizmoMode = gizmoMode == ImGuizmo::MODE::LOCAL ? ImGuizmo::MODE::WORLD : ImGuizmo::MODE::LOCAL;
	}
	ImGui::SameLine(); ImGui::PopStyleVar();

	// Grid Snap
	if (ImGuiHelper::HighlightedImageButton((ImTextureID)hGridSnapIcon.Get()->buffer, imageSize, gridSnapActive))
	{
		gridSnapActive = !gridSnapActive;
	}
	ImGui::SameLine();

	// Grid snap value
	ImVec2 buttonTextMinSize = imageSize + ImGui::GetStyle().FramePadding * 2;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.5f, 6.5f));

	std::string gridSnapText = std::to_string(gridSnapValues[gridSnapIndex].value);
	ImVec2 buttonSize = ImGui::CalcTextSize(gridSnapText.c_str()) + ImGui::GetStyle().FramePadding * 2;
	buttonSize.x = Clamp(buttonSize.x, buttonTextMinSize.x, buttonSize.x);
	if (ImGui::Button(gridSnapText.append("##GridSnap").c_str(), buttonSize))
	{
		ImGui::OpenPopup("Grid Snap Settings");
	}
	ImGui::PopStyleVar();

	// Rotation snap
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f)); ImGui::SameLine();
	if (ImGuiHelper::HighlightedImageButton((ImTextureID)hRotationSnapIcon.Get()->buffer, imageSize, rotationSnapActive))
	{
		rotationSnapActive = !rotationSnapActive;
	}
	ImGui::PopStyleVar(); 
	
	// Rotation snap value
	ImGui::SameLine();
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.5f, 6.5f));
	std::string rotationSnapText = std::to_string(rotationSnapValues[rotationSnapIndex].value);
	buttonSize = ImGui::CalcTextSize(rotationSnapText.c_str()) + ImGui::GetStyle().FramePadding * 2;
	buttonSize.x = Clamp(buttonSize.x, buttonTextMinSize.x, buttonSize.x);

	if (ImGui::Button(rotationSnapText.append("##RotationSnap").c_str(), buttonSize))
	{
		ImGui::OpenPopup("Rotation Snap Settings");
	}
	ImGui::PopStyleVar();

	// Scale snap
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f)); ImGui::SameLine();
	if (ImGuiHelper::HighlightedImageButton((ImTextureID)hScaleSnapIcon.Get()->buffer, imageSize, scaleSnapActive))
	{
		scaleSnapActive = !scaleSnapActive;
	}
	ImGui::PopStyleVar();

	// Scale snap value
	ImGui::SameLine();
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.5f, 6.5f));
	std::string scaleSnapText = std::to_string(scaleSnapValues[scaleSnapIndex].value).substr(0, scaleSnapValues[scaleSnapIndex].characters);
	buttonSize = ImGui::CalcTextSize(scaleSnapText.c_str()) + ImGui::GetStyle().FramePadding * 2;
	buttonSize.x = Clamp(buttonSize.x, buttonTextMinSize.x, buttonSize.x);

	if (ImGui::Button(scaleSnapText.append("##ScaleSnap").c_str(), buttonSize))
	{
		ImGui::OpenPopup("Scale Snap Settings");
	}
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

	if (ImGui::BeginPopup("Grid Snap Settings"))
	{
		ImGuiHelper::ValueSelection<GridSnapping>(gridSnapIndex, gridSnapValues);
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Rotation Snap Settings"))
	{
		ImGuiHelper::ValueSelection<RotationSnapping>(rotationSnapIndex, rotationSnapValues);
		ImGui::EndPopup();
	}
	
	if (ImGui::BeginPopup("Scale Snap Settings"))
	{
		ImGuiHelper::ValueSelection<ScaleSnapping>(scaleSnapIndex, scaleSnapValues);
		ImGui::EndPopup();
	}
	WViewport::DrawToolbarCustom();
}