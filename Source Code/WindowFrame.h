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
	virtual void LoadLayout_ForceDefault(Config& file, ImGuiID mainDockID) = 0;

	//TODO: moving to private as the code gets cleaner (used in M_Editor for the port)
	std::vector<Window*> windows;

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
};

#endif //__WINDOW_FRAME_H__
