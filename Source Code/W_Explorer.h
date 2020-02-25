#ifndef __W_EXPLORER_H__
#define __W_EXPLORER_H__

#include "DWindow.h"

#include "PathNode.h"
#include "Vec2.h"
#include "Timer.h"

class R_Prefab;

class W_Explorer : public DWindow
{
public:
	W_Explorer(M_Editor* editor);
	~W_Explorer() {}

	void Draw();
	void OnResize();

	const PathNode& GetCurrentNode() const { return currentNode; };

private:
	void DrawFolderNode(const PathNode& node);
	void DrawNodeImage(const PathNode& node);
	void DrawSelectedFolderContent();
	void DrawResourceItem();

	void UpdateTree();
	std::string GetTextAdjusted(const char* text);

	void HandleNodeDoubleClick(const PathNode& node);
	uint GetTextureFromNode(const PathNode& node, uint64* resource_id = nullptr, std::string* dnd_event = nullptr);

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
	PathNode currentNode;
	PathNode nextCurrent;
	PathNode explorerSelected;

	Vec2 explorerPosition;
	Vec2 explorerSize;

	uint updateTime = 5;
	Timer updateTimer;

	Vec2 windowSize;

	R_Prefab*  openPrefab = nullptr;
};

#endif