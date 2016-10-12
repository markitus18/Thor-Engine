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