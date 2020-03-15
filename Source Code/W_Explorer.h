#ifndef __W_EXPLORER_H__
#define __W_EXPLORER_H__

#include "DWindow.h"

#include "PathNode.h"
#include "Vec2.h"
#include "Timer.h"

class Resource;
class R_Prefab;
class R_Folder;

struct ImVec2;

class W_Explorer : public DWindow
{
public:
	W_Explorer(M_Editor* editor);
	~W_Explorer() {}

	void Draw();
	void OnResize();

	const Resource* GetCurrentFolder() const { return (Resource*)currentFolder; };

private:
	void DrawFolderNode(const PathNode& node);

	void DrawSelectedFolderContent();
	void DrawResourceItem(Resource* resource, uint& itemIndex, ImVec2 windowCursorPos);
	void DrawResourceImage(const Resource* resource);

	void UpdateTree();
	std::string GetTextAdjusted(const char* text) const;

	void HandleResourceDoubleClick(const Resource* resource);
	uint GetTextureFromResource(const Resource* resource, std::string* dnd_event = nullptr);

public:
	bool explorerActive = true;
	uint folderBuffer = 0;
	uint fileBuffer = 0;
	uint selectedBuffer = 0;
	uint sceneBuffer = 0;
	uint prefabBuffer = 0;
	uint shaderBuffer = 0;
	uint materialBuffer = 0;
	uint animationBuffer = 0;
	uint animatorBuffer = 0;

	uint imageSize = 64;
	uint columnsNumber = 0;
	uint imageSpacingX = 50;
	uint imageSpacingY = 50;
	uint textOffset = 6;
	uint topMarginOffset = 10;
	uint nodeButtonOffset = 30;

private:
	PathNode assets;
	R_Folder* currentFolder = nullptr;
	R_Folder* nextCurrentFolder = nullptr;
	Resource* selectedResource = nullptr;

	Vec2 explorerPosition;
	Vec2 explorerSize;

	uint updateTime = 5;
	Timer updateTimer;

	Vec2 windowSize;

	R_Prefab*  openPrefab = nullptr;
};

#endif