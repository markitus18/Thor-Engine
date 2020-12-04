#ifndef __RENDERING_FLAGS_H__

#define __RENDERING_FLAGS_H__

struct ERenderingFlags
{
	typedef unsigned __int64 Flags;
	enum
	{
		None =			0,
		Grid =			1 << 0,
		Skybox =		1 << 1,
		Quadtree =		1 << 2,
		Bounds_AABB =	1 << 3,
		Bounds_OBB =	1 << 4,
		Gizmo2D =		1 << 5,
		Frustum =		1 << 6,
		MousePick =		1 << 7,
		Bones =			1 << 8,
		Max =			1 << 9
	};
	inline static const std::string str[] = { "None", "Grid", "Skybox", "Quadtree", "Bounds_AABB", "Bounds_OBB", "Gizmo2D", "Frustum", "MousePick", "Bones", "Max" };
	inline static uint ToI(Flags flag) { return log2(flag) + 1; }

	static const Flags DefaultGameFlags =		Skybox;

	static const Flags DefaultEditorFlags =		Grid		| Skybox		| Gizmo2D |
												Frustum		| Bones;
};

struct EViewportCameraAngle
{
	typedef unsigned __int64 Flags;
	enum
	{
		None =			0,
		Perspective =	1 << 0,
		Top =			1 << 1,
		Bottom =		1 << 2,
		Left =			1 << 3,
		Right =			1 << 4,
		Front =			1 << 5,
		Back =			1 << 6,
		Max =			1 << 7
	};
	inline static const std::string str[] = { "None", "Perspective", "Top", "Bottom", "Left", "Right", "Front", "Back", "Max" };
	inline static uint ToI(Flags flag) { return log2(flag) + 1; }
};

struct EViewportViewMode
{
	typedef unsigned __int64 Flags;
	enum
	{
		None =		0,
		Lit =		1 << 0,
		Wireframe = 1 << 1,
		Unlit =		1 << 2,
		Max =		1 << 3
	};
	inline static const std::string str[] = { "None", "Lit", "Wireframe", "Unlit", "Max" };
	inline static uint ToI(Flags flag) { return log2(flag) + 1; }
};

struct EViewportStats
{
	typedef unsigned __int64 Flags;
	enum
	{
		None = 0,
		FPS = 1 << 0,
		MeshCount = 1 << 1,
		VertexCount = 1 << 2,
		Max = 1 << 3
	};
	inline static const std::string str[] = { "None", "FPS", "Mesh Count", "Triangle Count", "Max" };
	inline static uint ToI(Flags flag) { return log2(flag) + 1; }
};

#endif // __RENDERING_FLAGS_H__
