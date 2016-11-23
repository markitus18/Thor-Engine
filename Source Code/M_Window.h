#ifndef __M_Window_H__
#define __M_Window_H__

#include "Module.h"
#include "SDL/include/SDL.h"
#include <string>

class Application;
class Config;

class M_Window : public Module
{
public:

	M_Window(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~M_Window();

	bool Init(Config& config);
	bool CleanUp();

	void SetTitle(const char* title);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;
};

#endif // __M_Window_H__