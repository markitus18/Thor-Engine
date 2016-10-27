#ifndef __GIZMOS_H__
#define __GIZMOS_H__

#include "Globals.h"
#include "OpenGL.h"
#include "Color.h"
#include "MathGeoLib\src\MathGeoLib.h"

class Gizmos
{
public:
	static void DrawWireBox(const AABB& aabb, Color color);
	static void DrawWireBox(const OBB& obb, Color color);
	static void DrawWireFrustum(const Frustum& frustum, Color color);
	static void DrawWireCube(const float3* corners, Color color);
};

#endif //__GIZMOS_H__

