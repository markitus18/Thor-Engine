#ifndef __INTERSECTIONS_H__
#define __INTERSECTIONS_H__

#include "MathGeoLib\src\MathGeoLib.h"
#include "PerfTimer.h"

bool Intersects(const Frustum& frustum, const AABB& box, PerfTimer& timer);


#endif //__INTERSECTIONS_H__