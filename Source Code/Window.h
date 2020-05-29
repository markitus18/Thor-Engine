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
	Window(M_Editor* editor, std::string name, ImGuiWindowClass* windowClass);
	virtual ~Window();

	virtual void Draw() = 0;

	void SetActive(bool active);

	virtual void OnResize(Vec2 newSize) {};

	virtual void SaveConfig(Config& config);
	virtual void LoadConfig(Config& config);

	inline bool IsActive() const { return active; };

public:
	std::string name;
	M_Editor* editor;
	bool showDebugInfo = false;
	Vec2 windowSize = { 0, 0 };
	ImGuiWindowClass* windowClass = nullptr;

protected:
	bool active = true;
};

#endif //__DOCK_DATA_H__

