#ifndef __W_DETAILS_H__
#define __W_DETAILS_H__

#include "Window.h"

#include "ResourceHandle.h"

#include "ImGui/imgui.h" //Can we avoid including all ImGui just for Vec2?

class Resource;
class R_Texture;

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
	
	uint64 DrawDetails_Resource(const char* name, const Resource* resource);

private:
	float columnSeparator = 0.3f;
	bool separatorPressed = false;

	float separatorStartDragPosition = 0.0f;
	ImVec2 initItemCursor = ImVec2(0.0f, 0.0f);

	static ResourceHandle<R_Texture> hPreviewTexture;
};


#endif
