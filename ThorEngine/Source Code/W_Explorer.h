#ifndef __W_EXPLORER_H__
#define __W_EXPLORER_H__

#include "Window.h"

#include "ResourceHandle.h"

#include "PathNode.h"
#include "Vec2.h"
#include "Timer.h"

#include <map>

class Resource;

class R_Model;
class R_Folder;
class R_Texture;

struct ImVec2;
struct ImGuiWindowClass;

class W_Explorer : public Window
{
public:
	W_Explorer(WindowFrame* parent, ImGuiWindowClass* windowClass, ImGuiWindowClass* explorerWindowClass, int ID);
	~W_Explorer() {}

	void Draw() override;
	void OnResize(Vec2 newSize) override;
	void LoadLayout_Default();

	static inline const char* GetName() { return "Explorer"; }

private:
	void DrawFolderNode(PathNode& pathNode);

	void DrawSelectedFolderContent();
	void DrawResourceItem(const ResourceBase& base, uint& itemIndex, ImVec2 windowCursorPos);
	void DrawResourceImage(const ResourceBase& base);

	void UpdateTree();
	std::string GetTextAdjusted(const char* text) const;

	void HandleResourceDoubleClick(const ResourceBase& base);
	uint GetTextureFromResource(const ResourceBase& base, std::string* dnd_event = nullptr);

	void SetCurrentFolder(uint64 folderID);
	void SetOpenModel(uint64 modelID);

public:
	bool explorerActive = true;
	std::map<ResourceType, ResourceHandle<R_Texture>> hResourceIcons;
	
	// ImGui works with pixels as floats so we store them as floats to avoid conversion
	float imageSize = 64.0f;
	float columnsNumber = 0.0f;
	float imageSpacingX = 50.0f;
	float imageSpacingY = 50.0f;
	float textOffset = 6.0f;
	float topMarginOffset = 10.0f;
	float nodeButtonOffset = 30.0f;

private:
	ImGuiWindowClass* explorerWindowClass = nullptr;

	PathNode assets;
	PathNode engineAssets;

	ResourceHandle<R_Folder> hAssetsFolder;
	ResourceHandle<R_Folder> hEngineAssetsFolder;

	uint64 openModelID = 0;

	uint64 nextCurrentFolderID = 0;

	ResourceHandle<R_Texture> hResourceHighlight;

	std::map<uint64, ResourceHandle<R_Texture>> texturesInFolder;
	std::map<uint64, ResourceHandle<R_Texture>> texturesInModel;

	Resource* selectedResource = nullptr;

	Vec2 explorerPosition;
	Vec2 explorerSize;

	uint updateTime = 5;
	Timer updateTimer;

	Vec2 windowSize;
};

#endif