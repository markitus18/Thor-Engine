#ifndef __DOCK_DATA_H__
#define __DOCK_DATA_H__

#include <string>

class Dock;
typedef unsigned int uint;
class M_Editor;
class Config;

class DWindow
{
public:
	DWindow(M_Editor* editor, std::string name);
	~DWindow();

	virtual void Draw() = 0;

	void SetParent(Dock* dock);
	void SetActive(bool active);

	virtual void OnResize() {};

	virtual void SaveConfig(Config& config);
	virtual void LoadConfig(Config& config);

	Dock* GetParent() const;
	bool IsActive() const;

private:

public:
	uint child_index = 0;
	std::string name;
	M_Editor* editor;
	bool allowScrollbar = true;
	bool showDebugInfo = false;

protected:
	Dock* parent = nullptr;
	bool active = false;
};

#endif //__DOCK_DATA_H__

