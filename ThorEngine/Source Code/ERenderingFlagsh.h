#ifndef __RENDERING_FLAGS_H__
#define __RENDERING_FLAGS_H__

struct RenderingSettings
{
	typedef unsigned __int64 RenderingFlags;
	enum ERenderingFlags
	{
		None = 0,
		RenderFlag_Grid				= 1 << 0,
		RenderFlag_Skybox			= 1 << 1,
		RenderFlag_Quadtree			= 1 << 2,
		RenderFlag_Bounds_AABB		= 1 << 3,
		RenderFlag_Bounds_OBB		= 1 << 4,
		RenderFlag_2D_Gizmo			= 1 << 5,
		RenderFlag_Frustum			= 1 << 6,
		RenderFlag_MousePick		= 1 << 7,
		RenderFlag_Bones			= 1 << 8,
	};

	static const RenderingFlags DefaultGameFlags = RenderFlag_Skybox;
};


#endif // __RENDERING_FLAGS_H__
