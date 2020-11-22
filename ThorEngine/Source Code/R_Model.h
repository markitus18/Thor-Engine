#ifndef __R_MODEL_H__
#define __R_MODEL_H__

#include "Resource.h"

#include "MathGeoLib/src/Math/Quat.h"
#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/float4x4.h"

#include <vector>

class GameObject;

struct ModelNode
{
	ModelNode() {}
	ModelNode(uint64 ID, const char* name = "No name", const float3& translation = float3::zero, const float3& scale = float3::one, const Quat& rotation = Quat::identity, uint64 parentID = 0) :
	ID(ID), name(name), parentID(parentID), materialID(-1), meshID(-1)
	{
		transform = float4x4::FromTRS(translation, rotation, scale);
	}

	std::string name;
	uint64 ID;
	float4x4 transform;

	uint64 parentID;
	int meshID;
	int materialID;

	virtual void Serialize(Config& config)
	{
		config.Serialize("Node ID", ID);
		config.Serialize("Name", name);

		config.Serialize("Parent Node ID", parentID);

		config.SerializeArray("Transform", transform.ptr(), 16);

		config.Serialize("Mesh ID", meshID);
		config.Serialize("Material ID", materialID);
	}
};

class R_Model : public Resource
{
public:
	R_Model();
	~R_Model();

	void Serialize(Config& config);

	uint64 thumbnailID = 0;
	std::vector<ModelNode> nodes;
};

#endif //__R_MODEL_H__