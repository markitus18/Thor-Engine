#ifndef __DOCK_DATA_H__
#define __DOCK_DATA_H__

#include <string>
#include "Vec2.h"

class Dock;
typedef unsigned int uint;
class M_Editor;
class Config;
struct ImGuiWindowClass;

class Window
{
public:
	Window(M_Editor* editor, std::string name, ImGuiWindowClass* windowClass, int ID);
	virtual ~Window();

	virtual void Draw() = 0;

	void SetActive(bool active);

	virtual void OnResize(Vec2 newSize) {};

	virtual void SaveConfig(Config& config);
	virtual void LoadConfig(Config& config);

	inline bool IsActive() const { return active; }
	inline const char* GetWindowStrID() { return windowStrID.c_str(); }

public:
	std::string name;
	M_Editor* editor;
	bool showDebugInfo = false;
	Vec2 windowSize = { 0, 0 };
	ImGuiWindowClass* windowClass = nullptr;

protected:
	bool active = true;
	int ID = -1;
	std::string windowStrID;
};

#endif //__DOCK_DATA_H__

