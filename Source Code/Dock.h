#ifndef __DOCK_H__
#define __DOCK_H__

#include <vector>
#include <string>
#include "Vec2.h"

typedef unsigned int uint;
class DWindow;

struct Separator
{
	float position = 0.5f;
	bool pressed = false;
	float init_position = 0.5f;
};

enum Separation_Type
{
	NONE,
	VERTICAL,
	HORIZONTAL,
};

class Dock
{
public:
	Dock(const char* name, Vec2 size = Vec2(0, 0));
	~Dock();

	void Draw();
	void DrawSeparator();

	void AddChildData(DWindow* dock, int position = -1);
	void RemoveChildData(DWindow* dock);

	void Split(Separation_Type type, float pos = 0.5f);

	void Close();
	void ClearDockChildren();

	void SetDataActive(DWindow* data);
	void CloseDockData(DWindow* data);

	void SetSize(Vec2 size);

	std::vector<Dock*>& GetDockChildren();

private:
	void DrawTabPanels();
	void DrawSingleTab(DWindow* data);
	bool DoesTabFit(DWindow* dock);

	void UpdateChildrenPosition();
	void CapSeparatorPosition();
	float GetMinSize(Separation_Type sep_type);

public:
	std::string name = "dock";
	bool root = true;
	bool focused = false;

	Vec2 position;
	Vec2 size;
	Vec2 tabSpacing;

	float min_size = 150.0f;

	Separation_Type separation = NONE;
private:
	Dock* parent = nullptr;
	std::vector<DWindow*> data_children;
	std::vector<Dock*> dock_children;

	Separator separator;

	bool open = true;
};

#endif