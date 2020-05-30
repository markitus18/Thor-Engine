#ifndef __W_DETAILS_H__
#define __W_DETAILS_H__

#include "Window.h"

#include "MathGeoLib/src/Math/float3.h"
#include "ImGui/imgui.h"

class Resource;

struct ImGuiWindowClass;

class WDetails : public Window
{
public:
	WDetails(M_Editor* editor, const char* name, ImGuiWindowClass* windowClass, int ID);
	~WDetails() { }

	void BeginItemDraw(const char* name);
	void FinishItemDraw(const char* name);
	void SetNextItemMaxWidth(float maxWidth);
	
	bool DrawDetails_CheckBox(const char* name, bool& value);
	bool DrawDetails_Float(const char* name, float& value);
	bool DrawDetails_Float2(const char* name, float2& value);
	bool DrawDetails_Float3(const char* name, float3& value);
	bool DrawDetails_Resource(const char* name, Resource*& resource);

private:
	float columnSeparator = 0.3f;
	bool separatorPressed = false;

	float separatorStartDragPosition = 0.0f;
	ImVec2 initItemCursor = ImVec2(0.0f, 0.0f);

	static int previewImage;
};


#endif
