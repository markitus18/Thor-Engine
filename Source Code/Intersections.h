#ifndef __INTERSECTIONS_H__
#define __INTERSECTIONS_H__

#include "MathGeoLib\src\MathGeoLib.h"
#include "PerfTimer.h"
#include <vector>

bool Intersects(const Frustum& frustum, const AABB& box);
bool Intersects(const Plane* planes, const AABB& box, bool optimized = true);


#endif //__INTERSECTIONS_H__