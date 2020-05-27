#ifndef __WINDOW_FRAME_H__
#define __WINDOW_FRAME_H__

#include <vector>
#include <string>

class Window;
class Config;

class WindowFrame
{
public:
	WindowFrame();
	virtual ~WindowFrame();

	void Draw();

	inline void SetName(const char* name) { this->name = name; };
	inline const char* GetName() const { return name.c_str(); }

	void LoadLayout(Config& file);
	void SaveLayout(Config& file);

	//TODO: moving to private as the code gets cleaner (used in M_Editor for the port)
	std::vector<Window*> windows;

private:
	virtual void DrawMenuBar() { };

protected:
	std::string name;
};

#endif //__WINDOW_FRAME_H__
