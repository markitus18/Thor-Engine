#include "Intersections.h"
#include "Globals.h"

//TMP
#include "Application.h"
#include "ModuleEditor.h"
#include "PanelConfiguration.h"

bool Intersects(const Frustum& frustum, const AABB& box, uint timer)
{
	bool libMethod = false;
	bool optimized = false;
	if (libMethod)
	{
		if (frustum.Intersects(box))
		{
			return true;
		}
		return false;
	}
	else
	{
		Plane planes[6];
		frustum.GetPlanes(planes);
		if (!optimized)
		{
			float3 corners[8];
			box.GetCornerPoints(corners);


			for (uint p = 0; p < 6; p++)
			{
				int outSidePoints = 0;
				for (uint c = 0; c < 8; c++)
				{
					if (planes[p].normal.Dot(corners[c]) - planes[p].d >= 0.f)
					{
						outSidePoints++;
					}
					if (outSidePoints >= 8)
					{
						return false;
					}
				}
			}
			return true;
		}
		else
		{
			for (uint p = 0; p < 6; p++)
			{
				float3 farthestPoint = float3::zero;
				float3 normal = planes[p].normal;
				farthestPoint.x = normal.x >= 0 ? box.maxPoint.x : box.minPoint.x;
				farthestPoint.y = normal.y >= 0 ? box.maxPoint.y : box.minPoint.y;
				farthestPoint.z = planes[p].d >= 0 ? box.maxPoint.z : box.minPoint.z;

				if (planes[p].normal.Dot(farthestPoint) - planes[p].d >= 0.f)
				{
					return false;
				}
			}
			return true;

		}
	}
}

bool Intersects(const Plane* planes, const AABB& box, uint timer)
{
	bool optimized = true;

	if (!optimized)
	{
		float3 corners[8];
		box.GetCornerPoints(corners);

		for (uint p = 0; p < 6; p++)
		{
			int outSidePoints = 0;
			for (uint c = 0; c < 8; c++)
			{
				if (planes[p].normal.Dot(corners[c]) - planes[p].d >= 0.f)
				{
					outSidePoints++;
				}
				if (outSidePoints >= 8)
				{
					return false;
				}
			}
		}
		return true;
	}
	else
	{
		for (uint p = 0; p < 6; p++)
		{
			float3 farthestPoint = float3::zero;
			float3 normal = planes[p].normal;
			farthestPoint.x = -normal.x >= 0 ? box.maxPoint.x : box.minPoint.x;
			farthestPoint.y = -normal.y >= 0 ? box.maxPoint.y : box.minPoint.y;
			farthestPoint.z = -normal.z >= 0 ? box.maxPoint.z : box.minPoint.z;

			if (planes[p].normal.Dot(farthestPoint) - planes[p].d >= 0.f)
			{
				return false;
			}
		}
		return true;
	}
}