#ifndef __DOCK_DATA_H__
#define __DOCK_DATA_H__

#include <string>
#include "Vec2.h"

typedef unsigned int uint;

class Config;
class WindowFrame;

struct ImGuiWindowClass;

class Window
{
public:
	Window(WindowFrame* parent, std::string name, ImGuiWindowClass* windowClass, int ID);
	virtual ~Window();

	virtual void PrepareUpdate() {};
	virtual void Draw() = 0;

	inline void SetActive(bool active) {this->active = active; }

	virtual void OnResize(Vec2 newSize);

	virtual void SaveConfig(Config& config);
	virtual void LoadConfig(Config& config);

	inline bool IsActive() const { return active; }
	inline const char* GetWindowStrID() { return windowStrID.c_str(); }

public:
	std::string name;
	WindowFrame* parentFrame = nullptr;
	bool showDebugInfo = false;
	Vec2 windowSize = { 0, 0 };
	ImGuiWindowClass* windowClass = nullptr;

protected:
	bool active = true;
	int ID = -1;
	std::string windowStrID;
	bool isTabBarHidden;
};

#endif //__DOCK_DATA_H__

