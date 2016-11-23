#include "Application.h"
#include "M_Audio.h"

#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

M_Audio::M_Audio(bool start_enabled) : Module("Audio", start_enabled), music(nullptr)
{}

// Destructor
M_Audio::~M_Audio()
{}

// Called before render is available
bool M_Audio::Init(Config& config)
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s", SDL_GetError());
		ret = false;
	}

	// load support for the OGG format
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		ret = false;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
		ret = false;
	}

	return ret;
}

// Called before quitting
bool M_Audio::CleanUp()
{
	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music != nullptr)
	{
		Mix_FreeMusic(music);
	}

	std::vector<Mix_Chunk*>::iterator item;

	for(item = fx.begin(); item != fx.end(); item++)
	{
		Mix_FreeChunk(*item);
	}

	fx.clear();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	return true;
}

// Play a music file
bool M_Audio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;
	
	if(music != nullptr)
	{
		if(fade_time > 0.0f)
		{
			Mix_FadeOutMusic((int) (fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if(music == nullptr)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fade_time > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int M_Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if(chunk == nullptr)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
bool M_Audio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	Mix_Chunk* chunk = nullptr;
	chunk = fx[id - 1];

	if(chunk)
	{
		Mix_PlayChannel(-1, chunk, repeat);
		ret = true;
	}

	return ret;
}