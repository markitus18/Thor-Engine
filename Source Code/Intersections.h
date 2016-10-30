#ifndef __INTERSECTIONS_H__
#define __INTERSECTIONS_H__

#include "MathGeoLib\src\MathGeoLib.h"
#include "PerfTimer.h"
#include <vector>

bool Intersects(const Frustum& frustum, const AABB& box, uint timer = 0);
bool Intersects(const Plane* planes, const AABB& box, uint timer = 0);


#endif //__INTERSECTIONS_H__