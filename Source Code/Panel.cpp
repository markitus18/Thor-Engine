#include "Panel.h"

Panel::Panel()
{

}

Panel::Panel(int posX, int posY, int sizeX, int sizeY) : position(posX, posY), size(sizeX, sizeY)
{

}


Panel::~Panel()
{

}

void Panel::Draw(ImGuiWindowFlags flags)
{

}

void Panel::SetSize(int sizeX, int sizeY)
{
	size.x = sizeX;
	size.y = sizeY;
}

void Panel::SetPosition(int posX, int posY)
{
	position.x = posX;
	position.y = posY;
}

void Panel::UpdateSize(int, int)
{

}

void Panel::UpdatePosition(int, int)
{

}

SwapButtons::SwapButtons()
{
	nonActive_color[0] = ImVec4(0.67f, 0.40f, 0.40f, 0.60f);
	nonActive_color[1] = ImVec4(0.67f, 0.40f, 0.40f, 0.80f);
	nonActive_color[2] = ImVec4(0.67f, 0.40f, 0.40f, 1.00f);

	active_color[0] = ImVec4(0.40f, 0.67f, 0.40f, 0.60f);
	active_color[1] = ImVec4(0.40f, 0.67f, 0.40f, 0.80f);
	active_color[2] = ImVec4(0.40f, 0.67f, 0.40f, 1.00f);
}

void SwapButtons::AddButton(const char* text)
{
	buttons.push_back(text);
}

void SwapButtons::Draw()
{
	for (uint i = 0; i < buttons.size(); i++)
	{
		PushColors(active_button == i);

		if (ImGui::Button(buttons[i].c_str()))
		{
			active_button = i;
		}

		if (i < buttons.size() - 1)
		{
			ImGui::SameLine();
		}
		PopColors();
	}
}

const std::string& SwapButtons::GetActiveTag() const
{
	return buttons[active_button];
}

void SwapButtons::PushColors(bool active_button)
{
	const ImVec4* color = (active_button ? active_color : nonActive_color);

	ImGui::PushStyleColor(ImGuiCol_Button, color[0]);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color[1]);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, color[2]);
}

void SwapButtons::PopColors()
{
	ImGui::PopStyleColor(3);
}