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
	WindowFrame(const char* name, ImGuiWindowClass* frameWindowClass, ImGuiWindowClass* windowClass, int ID);
	virtual ~WindowFrame();

	void Draw();

	void SaveLayout(Config& file);
	void LoadLayout(Config& file, ImGuiID mainDockID);
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

public:
	bool requestLayoutSave = false;
	bool requestLayoutLoad = true;
	std::string layoutRequestName = "Default";

protected:
	//The name of the window frame. Used for saving layout information references
	std::string name;

	//The name that will be displayed in the window tab. Some windows will change names depending on the content
	//i.e: scene will display the name of the currently open scene
	std::string displayName;

	//Windows belonging in this particular window frame. All windows are added in the constructor, even if not active
	std::vector<Window*> windows;

	//Flag to allow docking/un-docking on this WindowFrame. The main window should not be moved or resized
	bool isDockable = true;

	//ImGui classes for blocking docking a frame window into a normal window and viceversa
	ImGuiWindowClass* frameWindowClass = nullptr;
	ImGuiWindowClass* windowClass = nullptr;

	//An Unique Identifier in order to have different WindowStrID sent to ImGui for the same type of window frames
	int ID = -1;
};

#endif //__WINDOW_FRAME_H__
