#include "R_Bone.h"
#include "MathGeoLib\src\MathGeoLib.h"

R_Bone::R_Bone() : Resource(Resource::Type::BONE)
{
	offset = float4x4::identity;
}

R_Bone::~R_Bone()
{

}