#include "Globals.h"
#include "Engine.h"
#include "M_Window.h"
#include "SDL\include\SDL_video.h"
#include "M_FileSystem.h"

#include "SDL/include/SDL.h"
#include "OpenGL.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )

M_Window::M_Window(bool start_enabled) : Module("Window", start_enabled)
{
	window = nullptr;
	screen_surface = nullptr;
	windowSize = Vec2(SCREEN_WIDTH, SCREEN_HEIGHT);
}

// Destructor
M_Window::~M_Window()
{
}

// Called before render is available
bool M_Window::Init(Config& config)
{

	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if (WIN_MAXIMIZED == true)
		{
			flags |= SDL_WINDOW_MAXIMIZED;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		flags |= SDL_WINDOW_ALLOW_HIGHDPI;

		window = SDL_CreateWindow(Engine->GetTitleName(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowSize.x, windowSize.y, flags);

		if(window == nullptr)
		{
			LOG("Window could not be created! SDL_Error: %s", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);

			//Get GL Context
			context = SDL_GL_CreateContext(window);
		}
	}

	return ret;
}

// Called before quitting
bool M_Window::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	SDL_GL_DeleteContext(context);

	//Destroy window
	if(window != nullptr)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void M_Window::SetTitle(const char* new_title)
{
	SDL_SetWindowTitle(window, new_title);
}