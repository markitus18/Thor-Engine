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
	RELEASE(root);
}

void Quadtree::Draw()
{
	root->Draw();
}

void Quadtree::AddGameObject(const GameObject* gameObject)
{
	if (root->AddGameObject(gameObject) == false)
		out_of_tree.push_back(gameObject);
}

bool Quadtree::RemoveGameObject(const GameObject* gameObject)
{
	if (root->RemoveGameObject(gameObject) == false)
	{
		for (std::vector<const GameObject*>::iterator it = out_of_tree.begin(); it < out_of_tree.end(); it++)
		{
			if (*it == gameObject)
			{
				out_of_tree.erase(it);
				return true;
			}
		}
	}
	return false;
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

bool QuadtreeNode::AddGameObject(const GameObject* gameObject)
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
			if (SendToChilds(gameObject))
				return true;
		}

	}
	else
	{
		return false;
	}


}

bool QuadtreeNode::RemoveGameObject(const GameObject* gameObject)
{
	for (std::vector<const GameObject*>::iterator it = bucket.begin(); it != bucket.end(); it++)
	{
		if (*it == gameObject)
		{
			bucket.erase(it);
			return true;
		}
	}

	for (uint i = 0; i < childs.size(); i++)
	{
		if (childs[i].RemoveGameObject(gameObject) == true)
			return true;
	}
	return false;
}

void QuadtreeNode::Redistribute()
{
	for (std::vector<const GameObject*>::iterator it = bucket.begin(); it != bucket.end();)
	{
		if (SendToChilds(*it))
		{
			it = bucket.erase(it);
		}
		else
		{
			it++;
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
		childs[intersectionChild].AddGameObject(gameObject);
		return true;
	}
	else if (intersectionCount == 0)
		LOG("[error] Quadtree parent node intersecting but not child intersection found");
	return false;
}

void QuadtreeNode::Draw()
{
	Color color;
	switch (bucket.size())
	{
	case 0:
		color = Color(0, 1, 0, 1);
		break;
	case 1:
		color = Color(1, 1, 0, 1);
		break;
	case 2:
		color = Color(1, 0, 0, 1);
		break;
	}
	App->renderer3D->AddAABB(box, color);

	for (uint i = 0; i < childs.size(); i++)
		childs[i].Draw();
}
