#ifndef __WINDOW_FRAME_H__
#define __WINDOW_FRAME_H__

#include <vector>
#include <string>

class Window;
class Config;
struct ImGuiDockNode;
struct ImGuiWindowClass;
typedef unsigned int ImGuiID;

class WindowFrame
{
public:
	WindowFrame(const char* name, ImGuiWindowClass* frameWindowClass, ImGuiWindowClass* windowClass);
	virtual ~WindowFrame();

	void Draw();

	void SaveLayout(Config& file);
	void LoadLayout(Config& file);
	virtual void LoadLayout_ForceDefault(Config& file, ImGuiID mainDockID) = 0;

	inline const char* GetName() { return name.c_str(); };

	Window* GetWindow(const char* name);

private:
	//Performs the entire management of drawing the menu bar
	void DrawMenuBar();

	//Generic File tab. Can be overriden in child classes
	virtual void MenuBar_File();

	//Generic Edit tab. Can be overriden in child classes
	virtual void MenuBar_Edit();

	//Any custom tabs needed for each specific window frame
	virtual void MenuBar_Custom() { };

	//Generic Window tab. Can be overriden in child classes
	virtual void MenuBar_Window();

	//Generic Help tab. Can be overriden in child classes
	virtual void MenuBar_Help();

	//Generic Development tab. Can be overriden in child classes
	virtual void MenuBar_Development() { };

	void SaveDockLayout(ImGuiDockNode* node, Config& file);
	void LoadDockLayout(ImGuiID dockID, Config& file);

protected:
	//The name of the window frame. Used for saving layout information references
	std::string name;

	//The name that will be displayed in the window tab. Some windows will change names depending on the content
	//i.e: scene will display the name of the currently open scene
	std::string displayName;

	//Windows belonging in this particular window frame. All windows are added in the constructor, even if not active
	std::vector<Window*> windows;

	ImGuiWindowClass* frameWindowClass = nullptr;
	ImGuiWindowClass* windowClass = nullptr;

};

#endif //__WINDOW_FRAME_H__
