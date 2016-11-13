#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include "MathGeoLib\src\MathGeoLib.h"
#include "Globals.h"

class GameObject;

class Quadtree
{
public:
	Quadtree();
	~Quadtree();

private:
	QuadtreeNode root;
};

class QuadtreeNode
{
public:
	QuadtreeNode();
	//Index marking which node from parent. 0 stats at top left, and counting clockwise
	QuadtreeNode(Quadtree* tree, QuadtreeNode* parent, uint index);
	~QuadtreeNode();

	void Split();
	void AddGameobject();

public:
	AABB box;
	std::vector<QuadtreeNode> childs;

	//Pointer to tree, maybe not necessary
	Quadtree* tree;
	uint maxBucketSize = 3;
	std::vector<GameObject*> gameObjects;
};

#endif