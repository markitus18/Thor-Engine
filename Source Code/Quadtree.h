#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include "MathGeoLib\src\MathGeoLib.h"
#include "Globals.h"

class GameObject;
class QuadtreeNode;

class Quadtree
{
public:
	Quadtree(const AABB& box);
	~Quadtree();
	void Draw();
	void AddGameObject(const GameObject* gameObject);
	bool RemoveGameObject(const GameObject* gameObject);

private:
	QuadtreeNode* root;
	std::vector<const GameObject*> out_of_tree;
};

class QuadtreeNode
{
	friend class Quadtree;

public:
	QuadtreeNode(const AABB& box);
	//Index marking which node from parent. 0 starts at top left, and counting clockwise
	QuadtreeNode(Quadtree* tree, QuadtreeNode* parent, uint index);
	~QuadtreeNode();

	bool AddGameObject(const GameObject* gameObject);
	bool RemoveGameObject(const GameObject* gameObject);

private:
	void Split();
	void Redistribute();
	bool SendToChilds(const GameObject* gameObject);
	void TryRemovingChilds();
	void GetChildsBuckets(std::vector<const GameObject*>& vector, bool addSelf) const;
	void Draw();

private:
	AABB box;
	AABB toDraw;
	std::vector<QuadtreeNode> childs;

	//Pointer to tree, maybe not necessary
	Quadtree* tree;
	uint maxBucketSize = 2;
	std::vector<const GameObject*> bucket;
};

#endif