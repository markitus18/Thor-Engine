#ifndef __M_Window_H__
#define __M_Window_H__

#include "Module.h"
#include "SDL/include/SDL.h"

#include "Vec2.h"
#include <string>

class Config;

class M_Window : public Module
{
public:

	M_Window(bool start_enabled = true);

	// Destructor
	~M_Window();

	bool Init(Config& config) override;
	bool CleanUp() override;

	void SetTitle(const char* title);

public:
	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The openGL context associated with this window
	SDL_GLContext context;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;

	//The size of the window
	Vec2 windowSize;
};

#endif // __M_Window_H__