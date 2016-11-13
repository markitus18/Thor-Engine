#include "Quadtree.h"
#include "GameObject.h"

Quadtree::Quadtree()
{

}

Quadtree::~Quadtree()
{

}

QuadtreeNode::QuadtreeNode()
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

	minPoint.x = (index / 2) == 1 ? parent->box.minPoint.x : (parent->box.maxPoint.x - parent->box.minPoint.x) / 2;
	maxPoint.x = (index / 2) == 1 ? (parent->box.maxPoint.x - parent->box.minPoint.x) / 2 : parent->box.maxPoint.x;

	minPoint.z = index % 2 == 0 ? parent->box.minPoint.z : (parent->box.maxPoint.z - parent->box.minPoint.z) / 2;
	maxPoint.z = index % 2 == 0 ? (parent->box.maxPoint.z - parent->box.minPoint.z) / 2 : parent->box.maxPoint.z;
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

