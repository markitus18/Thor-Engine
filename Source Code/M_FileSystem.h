#ifndef __MODULEFILESYSTEM_H__
#define __MODULEFILESYSTEM_H__

#include "Module.h"
#include <vector>

struct SDL_RWops;
int close_sdl_rwops(SDL_RWops *rw);

struct aiFileIO;

//struct BASS_FILEPROCS;
class Config;
struct PathNode;

class M_FileSystem : public Module
{
public:

	M_FileSystem(bool start_enabled = true);// const char* game_path = nullptr);

	// Destructor
	~M_FileSystem();

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
	void GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& file_list) const;
	PathNode GetAllFiles(const char* directory, std::vector<std::string>* filter_ext = nullptr, std::vector<std::string>* ignore_ext = nullptr) const;
	void GetRealDir(const char* path, std::string& output) const;
	std::string GetPathRelativeToAssets(const char* originalPath) const;

	bool HasExtension(const char* path) const;
	bool HasExtension(const char* path, std::string extension) const;
	bool HasExtension(const char* path, std::vector<std::string> extensions) const;

	std::string NormalizePath(const char* path) const;
	void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) const;

	// Open for Read/Write
	unsigned int Load(const char* path, const char* file, char** buffer) const;
	unsigned int Load(const char* file, char** buffer) const;
	SDL_RWops* Load(const char* file) const;

	bool DuplicateFile(const char* file, const char* dstFolder, std::string& relativePath);

	// IO interfaces for other libs to handle files via PHYSfs
	aiFileIO* GetAssimpIO();

	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;
	bool Remove(const char* file);

	uint64 GetLastModTime(const char* filename);

private:

	void CreateAssimpIO();

private:

	aiFileIO* AssimpIO = nullptr;
};

#endif // __MODULEFILESYSTEM_H__
