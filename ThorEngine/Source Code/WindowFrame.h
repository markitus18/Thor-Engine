#ifndef __WINDOW_FRAME_H__
#define __WINDOW_FRAME_H__

#include <vector>
#include <string>

#include "ResourceHandle.h"

class Window;
class Scene;
class Config;
class Resource;

struct ImGuiWindow;
struct ImGuiDockNode;
struct ImGuiWindowClass;

typedef unsigned int ImGuiID;
typedef unsigned int uint;
typedef unsigned __int64 uint64;

class WindowFrame
{
public:
	WindowFrame(const char* name, ImGuiWindowClass* frameWindowClass, ImGuiWindowClass* windowClass, int ID);
	virtual ~WindowFrame();

	virtual void PrepareUpdate();
	void Draw();

	virtual void LoadLayout_Default(ImGuiID mainDockID) = 0;

	inline const char* GetName() const { return name.c_str(); }
	inline const char* GetWindowStrID() const { return windowStrID.c_str(); }

	virtual void SetResource(uint64 resourceID);
	inline uint64 GetResourceID() const { return resourceHandle.GetID(); }
	inline uint GetID() const { return ID; };
	Window* GetWindow(const char* name) const;
	inline const std::vector<Window*> GetWindows() const { return windows; }

public:
	// Flag used to request a window close. When the window frame is closed by the user, we can safely destroy its memory
	bool requestClose = false;

	bool pendingLoadLayout = false;

	bool checkedForSave = false;

private:
	//Performs the entire management of drawing the menu bar
	void DrawMenuBar();

	//Generic File tab. Can be overridden in child classes
	virtual void MenuBar_File();

	//Generic Edit tab. Can be overridden in child classes
	virtual void MenuBar_Edit();

	//Any custom tabs needed for each specific window frame
	virtual void MenuBar_Custom() { };

	//Generic Window tab. Can be overridden in child classes
	virtual void MenuBar_Window();

	//Generic Help tab. Can be overridden in child classes
	virtual void MenuBar_Help();

	//Generic Development tab. Can be overridden in child classes
	virtual void MenuBar_Development() { };

protected:
	// The name of the window frame. Used for saving layout information references
	std::string name;

	// The name that will be displayed in the window tab. Some windows will change names depending on the content
	// i.e: scene will display the name of the currently open scene
	std::string displayName;

	// The string used as ID for ImGui's window call. It is a combination of the displayName, the name and the ID
	std::string windowStrID;

	// Windows belonging in this particular window frame. All windows are added in the constructor, even if not active
	std::vector<Window*> windows;

	// Flag to allow docking/undocking on this WindowFrame. The main window should not be moved or resized
	bool isDockable = true;

	// ImGui classes for blocking docking a frame window into a normal window and vice versa
	ImGuiWindowClass* frameWindowClass = nullptr;
	ImGuiWindowClass* windowClass = nullptr;

	// A Unique Identifier in order to have different WindowStrID sent to ImGui for the same type of window frames
	int ID = -1;

	// Most if not all windows will have a resource assigned to them (scene, particle editor, material editor,...)
	ResourceHandle<Resource> resourceHandle;

	// Most frames will hold a scene to display its content. Asides from the main frame, all frames will be owners of their scene
	Scene* scene = nullptr;
};

#endif //__WINDOW_FRAME_H__
