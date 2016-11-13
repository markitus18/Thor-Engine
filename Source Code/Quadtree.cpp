#include "Quadtree.h"
#include "GameObject.h"

#include "Application.h"
#include "ModuleRenderer3D.h"

Quadtree::Quadtree(const AABB& box)
{
	root = new QuadtreeNode(box);
	root->tree = this;
}

Quadtree::~Quadtree()
{

}

void Quadtree::Draw()
{
	root->Draw();
}

void Quadtree::AddGameObject(const GameObject* gameObject)
{
	root->AddGameobject(gameObject);
}

QuadtreeNode::QuadtreeNode(const AABB& box) : box(box)
{
}

QuadtreeNode::QuadtreeNode(Quadtree* tree, QuadtreeNode* parent, uint index) : tree(tree)
{
	//Index positions from top view
	//0 1
	//2 3
	vec minPoint, maxPoint;
	minPoint.y = parent->box.minPoint.y;
	maxPoint.y = parent->box.maxPoint.y;

	minPoint.x = (index / 2) == 1 ? parent->box.minPoint.x : (parent->box.maxPoint.x + parent->box.minPoint.x) / 2;
	maxPoint.x = (index / 2) == 1 ? (parent->box.maxPoint.x + parent->box.minPoint.x) / 2 : parent->box.maxPoint.x;

	minPoint.z = index % 2 == 0 ? parent->box.minPoint.z : (parent->box.maxPoint.z + parent->box.minPoint.z) / 2;
	maxPoint.z = index % 2 == 0 ? (parent->box.maxPoint.z + parent->box.minPoint.z) / 2 : parent->box.maxPoint.z;
	box = AABB(minPoint, maxPoint);
}

QuadtreeNode::~QuadtreeNode()
{
}

void QuadtreeNode::Split()
{
	if (!childs.empty())
		LOG("[error] Quadtree Node splitting when it already has childs");

	else
		for (uint i = 0; i < 4; i++)
			childs.push_back(QuadtreeNode(tree, this, i));
}

bool QuadtreeNode::AddGameobject(const GameObject* gameObject)
{
	if (box.Intersects(gameObject->GetAABB()))
	{
		if (childs.empty())
		{
			bucket.push_back(gameObject);
			if (bucket.size() > maxBucketSize)
			{
				Split();
				Redistribute();
			}
		}
		else
		{
			SendToChilds(gameObject);
		}

	}
	else
		return false;

}

void QuadtreeNode::Redistribute()
{
	for (std::vector<const GameObject*>::iterator it = bucket.begin(); it != bucket.end(); it++)
	{
		if (SendToChilds(*it))
		{
			it = bucket.erase(it);
		}
	}
}

bool QuadtreeNode::SendToChilds(const GameObject* gameObject)
{
	uint intersectionCount = 0;
	uint intersectionChild = -1;

	for (uint i = 0; i < childs.size(); i++)
	{
		if (childs[i].box.Intersects(gameObject->GetAABB()))
		{
			intersectionCount++;
			intersectionChild = i;
		}
	}
	if (intersectionCount == 1)
	{
		childs[intersectionChild].AddGameobject(gameObject);
		return true;
	}
	else if (intersectionCount == 0)
		LOG("[error] Quadtree parent node intersecting but not child intersection found");
	return false;
}

void QuadtreeNode::Draw()
{
	App->renderer3D->AddAABB(box, Color(0.6, 0.19, 0.8, 1));

	for (uint i = 0; i < childs.size(); i++)
		childs[i].Draw();
}
