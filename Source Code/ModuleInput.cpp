#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"

#include "SDL/include/SDL_mouse.h"
#include "Assimp/include/cfileio.h"
#include "Assimp/include/types.h"
#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module("Input", start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	RELEASE_ARRAY(keyboard);
}

// Called before render is available
bool ModuleInput::Init(Config& config)
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	static SDL_Event event;

	mouse_motion_x = mouse_motion_y = 0;

	const Uint8* keys = SDL_GetKeyboardState(nullptr);
	bool quit = false;

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else if (keyboard[i] != KEY_REPEAT)
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_z = 0;

	while(SDL_PollEvent(&event) != 0)
	{
		App->moduleEditor->GetEvent(&event);

		switch(event.type)
		{
			case SDL_MOUSEWHEEL:
			mouse_z = event.wheel.y;
			break;

			case SDL_MOUSEMOTION:
				mouse_x = event.motion.x / SCREEN_SIZE;
				mouse_y = event.motion.y / SCREEN_SIZE;

				mouse_motion_x = (event.motion.xrel / SCREEN_SIZE) - last_mouse_swap;
				mouse_motion_y = event.motion.yrel / SCREEN_SIZE;

				//TODO: more polite way to to this ?
				//TODO: work with ImGui mouse control
				if (infiniteHorizontal)
				{
					if (mouse_x > App->renderer3D->window_width - 10)
					{
						int last_x = mouse_x;
						App->input->SetMouseX(10);
						last_mouse_swap = mouse_x - last_x;
						ResetImGuiDrag();

					}
					else if (mouse_x < 10) 
					{
						int last_x = mouse_x;
						App->input->SetMouseX(App->renderer3D->window_width - 10);
						last_mouse_swap = mouse_x - last_x;
						ResetImGuiDrag();
					}
					else
						last_mouse_swap = 0;
				}
				else
				{
					last_mouse_swap = 0;
				}
			break;

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_WINDOWEVENT:
			{
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					App->renderer3D->OnResize(event.window.data1, event.window.data2);
					App->moduleEditor->OnResize(event.window.data1, event.window.data2);
				}
			}
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	infiniteHorizontal = false;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void ModuleInput::SetMouseX(int x)
{
	SDL_WarpMouseInWindow(App->window->window, x, mouse_y);
	mouse_x = x;
}

void ModuleInput::SetMouseY(int y)
{
	SDL_WarpMouseInWindow(App->window->window, mouse_x, y);
	mouse_y = y;
}

void ModuleInput::InfiniteHorizontal()
{
	infiniteHorizontal = true;
}

void ModuleInput::ResetImGuiDrag()
{
	//First update mouse position, otherwise in next frame mousePrev will
	//not be the updated version
	ImGui::GetIO().MousePos.x = mouse_x;
	ImGui::GetIO().MousePos.y = mouse_y;

	//Last mouse click position is set to where the mouse is
	ImGui::ResetMouseDragDelta(0);

	//Then we reset our imgui dragged item mouse values
	ImGui::GetCurrentContext()->ActiveIdIsJustActivated = true;

	///It looks so simple when it's done :'(
}