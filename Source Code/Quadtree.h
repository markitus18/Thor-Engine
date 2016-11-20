#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include "MathGeoLib\src\MathGeoLib.h"
#include "Globals.h"
#include <map>

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
	void Clear();
	template<typename PRIMITIVE>
	void CollectCandidates(std::vector<const GameObject*>& gameObjects, const PRIMITIVE& primitive)
	{
		root->CollectCandidates(gameObjects, primitive);
	}

	template<typename PRIMITIVE>
	void CollectCandidates(std::map<float, const GameObject*>& gameObjects, const PRIMITIVE& primitive)
	{
		root->CollectCandidates(gameObjects, primitive);
	}

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

	template<typename PRIMITIVE>
	void CollectCandidates(std::vector<const GameObject*>& gameObjects, const PRIMITIVE& primitive);

	template<typename PRIMITIVE>
	void CollectCandidates(std::map<float, const GameObject*>& gameObjects, const PRIMITIVE& primitive);

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

template<typename PRIMITIVE>
void QuadtreeNode::CollectCandidates(std::vector<const GameObject*>& gameObjects, const PRIMITIVE& primitive)
{
	if (primitive.Intersects(box))
	{
		for (uint i = 0; i < bucket.size(); i++)
		{
			gameObjects.push_back(bucket[i]);
		}

		for (uint i = 0; i < childs.size(); i++)
		{
			childs[i].CollectCandidates(gameObjects, primitive);
		}
	}
}

template<typename PRIMITIVE>
void QuadtreeNode::CollectCandidates(std::map<float, const GameObject*>& gameObjects, const PRIMITIVE& primitive)
{
	if (primitive.Intersects(box))
	{
		float hit_near, hit_far;
		for (uint i = 0; i < bucket.size(); i++)
		{
			if (primitive.Intersects(bucket[i]->GetOBB(), hit_near, hit_far))
				gameObjects[hit_near] = bucket[i];
		}
		for (uint i = 0; i < childs.size(); i++)
		{
			childs[i].CollectCandidates(gameObjects, primitive);
		}
	}
}


#endif