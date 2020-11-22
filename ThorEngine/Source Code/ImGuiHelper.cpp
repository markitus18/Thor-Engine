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