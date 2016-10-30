#ifndef __PANEL_H__
#define __PANEL_H__

#include "p2Point.h"
#include "ImGui\imgui.h"
#include "Color.h"
#include "Globals.h"
#include <vector>



class Panel
{
public:
	Panel();
	Panel(int, int, int, int);
	virtual ~Panel();

	virtual void Draw(ImGuiWindowFlags flags);

	void SetSize(int, int);
	void SetPosition(int, int);

	virtual void UpdateSize(int, int);
	virtual void UpdatePosition(int, int);

protected:
	p2Point<int> position;
	p2Point<int> size;

public:
	bool active = true;
};

class SwapButtons
{
public:
	uint active_button = 0;
	ImVec4 active_color;
	ImVec4 nonActive_color;

	std::vector<std::string> buttons;

public:
	SwapButtons();
	void AddButton(const char* text);
	void Draw();
};
#endif //__PANEL_H__
