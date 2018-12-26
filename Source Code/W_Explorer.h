#ifndef __W_EXPLORER_H__
#define __W_EXPLORER_H__

#include "DWindow.h"

#include "PathNode.h"
#include "Vec2.h"
#include "Timer.h"

class W_Explorer : public DWindow
{
public:
	W_Explorer(M_Editor* editor);
	~W_Explorer() {}

	void Draw();
	void OnResize();

private:
	void DrawFolderNode(const PathNode& node);
	void DrawNodeImage(const PathNode& node);
	void DrawSelectedFolderContent();

	void UpdateTree();
	std::string GetTextAdjusted(const char* text);

	void HandleNodeDoubleClick(const PathNode& node);

public:
	bool explorerActive = true;
	uint folderBuffer = 0;
	uint fileBuffer = 0;
	uint selectedBuffer = 0;

	uint imageSize = 64;
	uint columnsNumber = 0;
	uint imageSpacingX = 50;
	uint imageSpacingY = 50;
	uint textOffset = 10;

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
};

#endif