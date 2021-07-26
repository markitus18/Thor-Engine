#include "ImGuiHelper.h"

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/imgui_internal.h"

void ImGuiHelper::PushInvisibleButtonStyle()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
}

void ImGuiHelper::PopInvisibleButtonStyle()
{
	ImGui::PopStyleVar(1);
}

bool ImGuiHelper::HighlightedImageButton(ImTextureID textureID, ImVec2 imageSize, bool highlight)
{
	if (highlight)
		PushButtonBackgroundHighlight();
	bool ret = ImGui::ImageButton(textureID, imageSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
	if (highlight)
		ImGuiHelper::PopButtonBackgroundHighlight();
	return ret;
}

void ImGuiHelper::PushButtonBackgroundHighlight()
{
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.78f, 0.78f, 0.0f, 0.4f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.78f, 0.78f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.7f, 0.0f, 1.0f));
}

void ImGuiHelper::PopButtonBackgroundHighlight()
{
	ImGui::PopStyleColor(3);
}