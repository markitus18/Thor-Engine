#include "Intersections.h"
#include "Globals.h"

//TMP
#include "Application.h"
#include "ModuleEditor.h"
#include "PanelConfiguration.h"

bool Intersects(const Frustum& frustum, const AABB& box, PerfTimer& timer)
{
	bool libMethod = false;
	bool optimized = true;
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
			//LOG("Getting Corners Time: %f", timer.ReadMs());

			timer.Start();
			for (uint p = 0; p < 6; p++)
			{
				int outSidePoints = 0;
				for (uint c = 0; c < 8; c++)
				{
					if (planes[p].IsOnPositiveSide(corners[c]))
					{
						outSidePoints++;
					}
					App->moduleEditor->panelConfiguration->timerRead = timer.ReadMs();
					if (outSidePoints >= 8)
					{
							App->moduleEditor->panelConfiguration->timerRead = timer.ReadMs();
							//	LOG("Intersection loop time: %f", timer.ReadMs());
						return false;
					}
				}
			}
			App->moduleEditor->panelConfiguration->timerRead = timer.ReadMs();
			return true;
		}
		else
		{
			timer.Start();
			for (uint p = 0; p < 6; p++)
			{
				float3 farthestPoint = float3::zero;
				float3 normal = planes[p].normal;
				farthestPoint.x = normal.x >= 0 ? box.maxPoint.x : box.minPoint.x;
				farthestPoint.y = normal.y >= 0 ? box.maxPoint.y : box.minPoint.y;
				farthestPoint.z = planes[p].d >= 0 ? box.maxPoint.z : box.minPoint.z;

				if (planes[p].IsOnPositiveSide(farthestPoint))
				{
					App->moduleEditor->panelConfiguration->timerRead = timer.ReadMs();
					return false;
				}
			}
			App->moduleEditor->panelConfiguration->timerRead = timer.ReadMs();
			return true;

		}
	}
		//LOG("Getting Planes Time: %f", timer.ReadMs());

}