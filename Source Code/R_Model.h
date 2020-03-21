#ifndef __R_MODEL_H__
#define __R_MODEL_H__

#include "Resource.h"

#include "MathGeoLib/src/Math/float4x4.h"
#include <vector>

class GameObject;

struct Node
{
	std::string name;
	uint ID;
	float4x4 transform;

	uint parentID;
	uint meshID;
	uint materialID;
};

class R_Model : public Resource
{
public:
	R_Model();
	~R_Model();

	GameObject* root = nullptr; //By now, just to compile. Should be erased with new structure

	uint64 thumbnailID = 0;
	std::vector<Node> nodes;
	std::vector<uint> animationIDs;
};

#endif //__R_MODEL_H__