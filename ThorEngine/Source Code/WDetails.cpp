#include "WDetails.h"

#include "Engine.h"
#include "M_Input.h"

#include "M_Resources.h"
#include "Resource.h"
#include "R_Texture.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui_internal.h"

#include "MathGeoLib/src/Math/float2.h"
#include "MathGeoLib/src/Math/float3.h"

ResourceHandle<R_Texture> WDetails::hPreviewTexture;

WDetails::WDetails(WindowFrame* parent, const char* name, ImGuiWindowClass* windowClass, int ID) : Window(parent, name, windowClass, ID)
{
	hPreviewTexture.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/PreviewIcon.png")->ID);
}

void WDetails::BeginItemDraw(const char* name)
{
	ImGui::PushID(name);

	initItemCursor = ImGui::GetCursorPos();
	ImVec2 initialRightCursor = initItemCursor + ImVec2(ImGui::GetWindowWidth() * columnSeparator, 0);
	ImGui::SetCursorPos(initialRightCursor);
}

void WDetails::FinishItemDraw(const char* name)
{
	ImVec2 finalRightCursor = ImGui::GetCursorPos();
	ImVec2 centerCursor = ImVec2(initItemCursor.x, initItemCursor.y + (finalRightCursor.y - initItemCursor.y) / 2);
	centerCursor.y -= ImGui::GetTextLineHeightWithSpacing() / 2;

	ImGui::SetCursorPos(centerCursor);
	ImGui::Text(name);
	ImGui::SetCursorPos(ImVec2(initItemCursor.x, finalRightCursor.y));

	//Setting the cursor position 5 pixels before the right side of the item starts
	ImVec2 initialRightCursor = initItemCursor + ImVec2(ImGui::GetWindowWidth() * columnSeparator, 0);
	ImVec2 initialButtonCursor = initialRightCursor - ImVec2(5, 4);
	ImGui::SetCursorPos(initialButtonCursor);

	ImVec2 buttonSize(3, finalRightCursor.y - initItemCursor.y + 4);
	ImGui::Button("", buttonSize);

	if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0) && !separatorPressed)
	{
		//TODO: Handle column separator movement
	}


	ImGui::PopID();
}

void WDetails::SetNextItemMaxWidth(float maxWidth)
{
	if (ImGui::CalcItemWidth() > maxWidth)
		ImGui::SetNextItemWidth(maxWidth);
}

bool WDetails::DrawDetails_CheckBox(const char* name, bool& value)
{
	bool ret = false;

	BeginItemDraw(name);
	if (ImGui::Checkbox("", &value))
		ret = true;
	FinishItemDraw(name);

	return ret;
}

bool WDetails::DrawDetails_Float(const char* name, float& value)
{
	bool ret = false;

	BeginItemDraw(name);
	SetNextItemMaxWidth(120.0f);
	if (ImGui::DragFloat("", &value))
	{
		Engine->input->InfiniteHorizontal();
		ret = true;
	}
	FinishItemDraw(name);

	return ret;
}

bool WDetails::DrawDetails_Float2(const char* name, float2& value)
{
	bool ret = false;

	BeginItemDraw(name);
	SetNextItemMaxWidth(244.0f);
	if (ImGui::DragFloat2("", value.ptr()))
	{
		Engine->input->InfiniteHorizontal();
		ret = true;
	}
	FinishItemDraw(name);

	return ret;
}

bool WDetails::DrawDetails_Float3(const char* name, float3& value)
{
	bool ret = false;

	BeginItemDraw(name);
	SetNextItemMaxWidth(368.0f);
	if (ImGui::DragFloat3("", value.ptr()))
	{
		Engine->input->InfiniteHorizontal();
		ret = true;
	}
	FinishItemDraw(name);

	return ret;
}

uint64 WDetails::DrawDetails_Resource(const char* name, const Resource* resource, const ResourceType resourceType)
{
	uint64 ret = 0;

	BeginItemDraw(name);

	ImGui::BeginGroup();
	ImVec2 prevCursor = ImGui::GetCursorPos();
	ImGui::Image((ImTextureID)hPreviewTexture.Get()->buffer, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SetCursorPos(prevCursor + ImVec2(64 + 10, 12));

	if (resource != nullptr)
	{
		ImGui::Text(resource->GetName());
	}

	ImGui::SetCursorPos(prevCursor + ImVec2(64 + 10, 12 + ImGui::GetTextLineHeightWithSpacing()));
	ImGui::Button("A"); ImGui::SameLine();
	ImGui::Button("B"); ImGui::SameLine();
	ImGui::Button("R");
	ImGui::EndGroup();

	if (ImGui::BeginDragDropTarget())
	{
		std::string dnd_event = std::string("DND_RESOURCE_") + std::to_string((int)resourceType);
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(dnd_event.c_str()))
		{
			if (payload->DataSize == sizeof(uint64))
			{
				uint64 resourceID = *(const uint64*)payload->Data;
				ret = resourceID;
			}
		}
		ImGui::EndDragDropTarget();
	}

	FinishItemDraw(name);

	return ret;
}