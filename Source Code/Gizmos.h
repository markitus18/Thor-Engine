#ifndef __GIZMOS_H__
#define __GIZMOS_H__

#include "Globals.h"
#include "OpenGL.h"
#include "Color.h"
#include "MathGeoLib\src\MathGeoLib.h"

class Gizmos
{
public:
	template <typename Box>
	static void DrawWireBox(const Box& box, Color color)
	{
		float3 corners[8];
		box.GetCornerPoints(corners);
		DrawWireCube(corners, color);
	}
	//static void DrawWireBox(const OBB& obb, Color color);
	//static void DrawWireFrustum(const Frustum& frustum, Color color);
	static void DrawWireCube(const float3* corners, Color color);
};

#endif //__GIZMOS_H__

