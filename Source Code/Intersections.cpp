#include "Intersections.h"
#include "Globals.h"

bool Intersects(const Frustum& frustum, const AABB& box)
{
	if (frustum.Intersects(box))
	{
		return true;
	}
	return false;
	//Plane planes[6];
	//frustum.GetPlanes(planes);

	//float3 corners[8];
	//box.GetCornerPoints(corners);

	//for (uint p = 0; p < 6; p++)
	//{
	//	int outSidePoints = 0;
	//	for (uint c = 0; c < 8; c++)
	//	{
	//		if (planes[p].IsOnPositiveSide(corners[c]))
	//		{
	//			outSidePoints++;
	//		}
	//		if (outSidePoints >= 8)
	//		{
	//			return false;
	//		}
	//	}
	//}
	//return true;
}