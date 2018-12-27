#include "Dock.h"

#include "ImGui\imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui\imgui_internal.h"

#include "DWindow.h"

Dock::Dock(const char* name, Vec2 size): size(size), name(name)
{

}

Dock::~Dock()
{

}

void Dock::Draw()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoResize;

	flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	if (root == true)
	{
		ImGui::SetNextWindowPos(ImVec2(position.x, position.y));
		ImGui::SetNextWindowSize(ImVec2(size.x, size.y));
		ImGui::Begin(name.c_str(), &open, flags);
	}
	else
	{
		ImGui::PushID(this);
		ImGui::BeginChild("Child", ImVec2(size.x, size.y), false, flags);

	}
	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && dock_children.size() == 0)
	{
		//editor->SetDockFocus(this);
	}
	switch (separation)
	{
		case(NONE):
		{
			if (data_children.empty()) break;

			tabSpacing = Vec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y);
			DrawTabPanels();
			ImGui::Separator();
			tabSpacing = Vec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY()) - tabSpacing;

			for (uint i = 0; i < data_children.size(); ++i)
			{
				if (data_children[i]->IsActive())
				{
					//Generate a child window then call draw from the window itself
					ImGui::PushID(data_children[i]);
					ImGui::BeginChild("DockChild", ImVec2(size.x, size.y - tabSpacing.y), false, data_children[i]->allowScrollbar ? 0 : ImGuiWindowFlags_NoScrollbar);

					data_children[i]->Draw();

					ImGui::EndChild();
					ImGui::PopID();
				}

			}
			break;
		}
		default:
		{
			if (dock_children.size() == 0) break;

			dock_children[0]->Draw();

 			if (separation == VERTICAL)	ImGui::SameLine(0, 0);
			ImVec2 separator_pos = ImGui::GetCursorPos();
			ImGui::SetCursorPos(separator_pos + (separation == VERTICAL ? ImVec2(7, 0) : ImVec2(0, 7)));
	
			dock_children[1]->Draw();

			ImGui::SetCursorPos(separator_pos);
			DrawSeparator();
			break;
		}
	}
	if (root == true)
	{
		ImGui::End();
	}
	else
	{
		ImGui::EndChild();
		ImGui::PopID();
	}
}

void Dock::DrawSeparator()
{
	ImVec2 button_size = (separation == VERTICAL ?	ImVec2(5, size.y ) :
														ImVec2(size.x, 5));
	ImGui::Button("b", button_size);

	if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0))
	{
		if (separator.pressed == false)
		{
			separator.pressed = true;
			separator.init_position = separator.position;
		}
	}
	else if (separator.pressed == true && !ImGui::IsMouseDown(0))
	{
		separator.pressed = false;
	}
	if (separator.pressed == true)
	{
		ImVec2 delta = ImGui::GetMouseDragDelta(0);
		float init_position = separator.init_position * (separation == VERTICAL ? size.x : size.y);
		float final_pos = separation == VERTICAL ? init_position + delta.x : init_position + delta.y;
		separator.position = final_pos / (separation == VERTICAL ? size.x : size.y);
		CapSeparatorPosition();
		UpdateChildrenPosition();
	}
}

void Dock::DrawTabPanels()
{
	for (int i = 0; i < data_children.size(); i++)
	{
		DrawSingleTab(data_children[i]);
	}
}

void Dock::DrawSingleTab(DWindow* data)
{
	ImGui::SameLine(0, 15);

	if (!DoesTabFit(data))
	{
		ImGui::NewLine();
		ImGui::NewLine();
		ImGui::SameLine(0, 15);
	}

	float line_height = ImGui::GetTextLineHeightWithSpacing();
	ImVec2 size(ImGui::CalcTextSize(data->name.c_str(), data->name.c_str() + data->name.length()).x, line_height);
	if (ImGui::InvisibleButton(data->name.c_str(), size))
	{
		SetDataActive(data);
	}

	bool hovered = ImGui::IsItemHovered();

	if (hovered && ImGui::IsMouseClicked(2))
	{
		RemoveChildData(data);
	}

	ImU32 color = ImGui::GetColorU32(ImGuiCol_FrameBg);
	ImU32 color_active = ImGui::GetColorU32(ImGuiCol_FrameBgActive);
	ImU32 color_hovered = ImGui::GetColorU32(ImGuiCol_FrameBgHovered);
	ImU32 color_dock_active = ImGui::GetColorU32(ImVec4(0.85, 0.65, 0.68, 0.8));

	ImVec2 pos = ImGui::GetItemRectMin();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	draw_list->PathClear();
	draw_list->PathLineTo(pos + ImVec2(-15, size.y));
	draw_list->PathBezierCurveTo(
		pos + ImVec2(-10, size.y), pos + ImVec2(-5, 0), pos + ImVec2(0, 0), 10);
	draw_list->PathLineTo(pos + ImVec2(size.x, 0));
	draw_list->PathBezierCurveTo(pos + ImVec2(size.x + 5, 0),
		pos + ImVec2(size.x + 10, size.y),
		pos + ImVec2(size.x + 15, size.y),
		10);
	draw_list->PathFillConvex(focused ? color_dock_active : (hovered ? color_hovered : (data->IsActive() ? color_active : color)));
	draw_list->AddText(pos, ImGui::GetColorU32(ImGuiCol_Text), data->name.c_str(), nullptr);
}

void Dock::AddChildData(DWindow* data, int position)
{
	if (dock_children.size() == 0)
	{
		data->SetParent(this);
		if (position == -1 || position > data_children.size())
			data_children.push_back(data);
		else
			data_children.insert(data_children.begin() + position, data);
		SetDataActive(data_children[0]);
	}
	else
	{
		dock_children[0]->AddChildData(data, position);
	}
}

bool Dock::DoesTabFit(DWindow* data)
{
	ImVec2 cursor_pos = ImGui::GetCursorPos();
	ImVec2 size(ImGui::CalcTextSize(data->name.c_str(), data->name.c_str() + data->name.length()).x, 0);
	return (cursor_pos.x + size.x + 15 < ImGui::GetWindowSize().x);
}

void Dock::UpdateChildrenPosition()
{
	if (dock_children.size() > 0)
	{
		if (separation == VERTICAL)
		{
			dock_children[0]->SetSize(Vec2(separator.position * size.x, size.y));
			dock_children[1]->SetSize(Vec2(size.x - (separator.position * size.x), size.y));
		}
		else
		{
			dock_children[0]->SetSize(Vec2(size.x, size.y * separator.position));
			dock_children[1]->SetSize(Vec2(size.x, size.y - (size.y * separator.position)));
		}
	}
}

void Dock::CapSeparatorPosition()
{
	float separator_pos = separator.position * (separation == VERTICAL ? size.x : size.y);
	float axis_size = (separation == VERTICAL ? size.x : size.y);

	float min_size = dock_children[0]->GetMinSize(separation);
	float max_size = axis_size - dock_children[1]->GetMinSize(separation);

	if (separator_pos < min_size) separator_pos = min_size;
	if (separator_pos > max_size) separator_pos = max_size;

	separator.position = separator_pos / (separation == VERTICAL ? size.x : size.y);
}

float Dock::GetMinSize(Separation_Type sep_type)
{
	if (dock_children.size() == 0)
	{
		return min_size;
	}

	float final_min_size = 0;
	for (uint i = 0; i < dock_children.size(); ++i)
	{
		if (sep_type != separation)
		{
			float child_size = dock_children[i]->GetMinSize(sep_type);
			if (child_size > final_min_size)
				final_min_size = child_size;
		}
		else
		{
			final_min_size += dock_children[i]->GetMinSize(sep_type);
		}
	}
	return final_min_size;

}
void Dock::RemoveChildData(DWindow* dock)
{
	for (std::vector<DWindow*>::iterator it = data_children.begin(); it != data_children.end(); ++it)
	{
		if (*it == dock)
		{
			data_children.erase(it);
			break;
		}
	}
	if (data_children.size() == 0)
	{

	}
}

void Dock::Split(Separation_Type type, float pos)
{
	separation = type;
	separator.position = pos;

	ClearDockChildren();
	dock_children.push_back(new Dock("Child 1"));
	dock_children.push_back(new Dock("Child 2"));

	while(data_children.size() > 0)
	{
		dock_children[0]->AddChildData(data_children[0]);
		data_children.erase(data_children.begin());
	}
	dock_children[0]->root = false;

	UpdateChildrenPosition();

	dock_children[1]->root = false;
	dock_children[0]->parent = this;
	dock_children[1]->parent = this;
}

void Dock::Close()
{
	/*
	if (childs.size() > 0)
	{
		childs[0]->root = true;
		for (uint i = 1; i < childs.size(); ++i)
		{
			childs[0]->AddChild(childs[i]);
		}
		if (active) childs[0]->SetActive(true);
	}
	if (parent != nullptr)
	{
		parent->RemoveChild(this);
	}
	root = false;
	*/
}

void Dock::ClearDockChildren()
{
	for (uint i = 0; i < dock_children.size(); ++i)
	{
		delete dock_children[i];
	}
	dock_children.clear();
}

void Dock::SetDataActive(DWindow* data)
{
	for (uint i = 0; i < data_children.size(); ++i)
	{
		data_children[i]->SetActive(data == data_children[i]);
	}
}

void Dock::CloseDockData(DWindow* data)
{
	std::vector<DWindow*>::iterator it;
	for (it = data_children.begin(); it != data_children.end(); ++it)
	{
		if ((*it) == data)
		{
			data_children.erase(it);
			data->SetActive(false);
		}
	}
}

void Dock::SetSize(Vec2 size)
{
	this->size = size - Vec2(6, 9); //Temoral fix to allow the scrollbar to appear
									//ImGui window size doesn't have the exact value. 

	for (uint i = 0; i < data_children.size(); ++i)
	{
		data_children[i]->OnResize();
	}

	if (dock_children.size() > 0)
	{
		CapSeparatorPosition();
		UpdateChildrenPosition();
	}

}

std::vector<Dock*>& Dock::GetDockChildren()
{
	return dock_children;
}