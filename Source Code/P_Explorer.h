#ifndef  __PANEL_EXPLORER_H__
#define __PANEL_EXPLORER_H__

#include "Panel.h"
#include "Timer.h"
#include "PathNode.h"

class P_Explorer : public Panel
{
public:
	P_Explorer();
	~P_Explorer();

	void Draw(ImGuiWindowFlags flags);
	void UpdatePosition(int, int);

private:
	void DrawFolderNode(const PathNode& node, ImGuiTreeNodeFlags flags);
	void DrawNodeImage(const PathNode& node);
	void UpdateTree();
	std::string GetTextAdjusted(const char* text);

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
	PathNode explorerSelected;

	ImVec2 explorerPosition;
	ImVec2 explorerSize;

	uint updateTime = 5;
	Timer updateTimer;
	
};

#endif // ! __PANEL_EXPLORER_H__

