#ifndef __PATHNODE_H__
#define __PATHNODE_H__

#include <string>
#include <vector>

struct PathNode
{
	PathNode() : path("") {};
	//Path in reference to the first PathNode
	std::string path;
	//Folder / file itself
	std::string localPath;

	std::vector<PathNode> children;

	bool isLeaf = true;
	bool isFile = true;

	bool IsLastFolder() const
	{
		for (uint i = 0; i < children.size(); i++)
			if (children[i].isFile == false)
				return false;
		return true;
	}

	bool operator ==(const PathNode node) const
	{
		return path == node.path;
	}
};

#endif
