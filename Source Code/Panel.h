#ifndef __PANEL_H__
#define __PANEL_H__

#include "p2Point.h"
#include "ImGui\imgui.h"

class Panel
{
public:
	Panel();
	Panel(int, int, int, int);
	virtual ~Panel();

	virtual void Draw();

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

#endif //__PANEL_H__
