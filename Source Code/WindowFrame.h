#ifndef __WINDOW_FRAME_H__
#define __WINDOW_FRAME_H__

#include <vector>
#include <string>

class Window;
class Config;
struct ImGuiDockNode;
typedef unsigned int ImGuiID;

class WindowFrame
{
public:
	WindowFrame();
	virtual ~WindowFrame();

	void Draw();

	inline void SetName(const char* name) { this->name = name; };
	inline const char* GetName() const { return name.c_str(); }

	void SaveLayout(Config& file);
	void LoadLayout(Config& file);
	void LoadLayout_ForceDefault(Config& file);

	//TODO: moving to private as the code gets cleaner (used in M_Editor for the port)
	std::vector<Window*> windows;

private:
	virtual void DrawMenuBar() { };

	void SaveDockLayout(ImGuiDockNode* node, Config& file);
	void LoadDockLayout(ImGuiID dockID, Config& file);

protected:
	std::string name;
};

#endif //__WINDOW_FRAME_H__
