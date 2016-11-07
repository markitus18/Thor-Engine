#ifndef __MODULEFILESYSTEM_H__
#define __MODULEFILESYSTEM_H__

#include "Module.h"
#include <vector>

struct SDL_RWops;
int close_sdl_rwops(SDL_RWops *rw);

struct aiFileIO;

//struct BASS_FILEPROCS;
class Config;

class ModuleFileSystem : public Module
{
public:

	ModuleFileSystem(Application* app, bool start_enabled = true);// const char* game_path = nullptr);

	// Destructor
	~ModuleFileSystem();

	// Called before render is available
	bool Init(Config& config);

	// Called before quitting
	bool CleanUp();

	// Utility functions
	bool AddPath(const char* path_or_zip);
	bool Exists(const char* file) const;
	bool CreateDir(const char* dir);
	bool IsDirectory(const char* file) const;
	const char* GetWriteDir() const;
	void DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) const;

	void NormalizePath(char* path) const;
	void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) const;

	// Open for Read/Write
	unsigned int Load(const char* path, const char* file, char** buffer) const;
	unsigned int Load(const char* file, char** buffer) const;
	SDL_RWops* Load(const char* file) const;

	// IO interfaces for other libs to handle files via PHYSfs
	aiFileIO* GetAssimpIO();

	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;
	bool Remove(const char* file);

private:

	void CreateAssimpIO();

private:

	aiFileIO* AssimpIO = nullptr;
};

#endif // __MODULEFILESYSTEM_H__
