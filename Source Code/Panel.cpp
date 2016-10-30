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
	active_color = ImVec4(0, 1, 0, 1);
	nonActive_color = ImVec4(1, 0, 0, 1);
}

void SwapButtons::AddButton(const char* text)
{
	buttons.push_back(text);
}

void SwapButtons::Draw()
{
	for (uint i = 0; i < buttons.size(); i++)
	{
		if (i == active_button)
			ImGui::PushStyleColor(ImGuiCol_Button, active_color);
		else
			ImGui::PushStyleColor(ImGuiCol_Button, nonActive_color);

		if (ImGui::Button(buttons[i].c_str()))
		{
			active_button = i;
		}

		if (i < buttons.size() - 1)
		{
			ImGui::SameLine();
		}
		ImGui::PopStyleColor();
	}
}