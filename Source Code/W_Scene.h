#ifndef __W_SCENE_H__
#define __W_SCENE_H__

#include "Window.h"
#include "Vec2.h"
#include "ImGuizmo/ImGuizmo.h"

struct ImGuiWindowClass;

class W_Scene : public Window
{
public:
	W_Scene(M_Editor* editor, ImGuiWindowClass* windowClass);
	~W_Scene() {}

	void Draw() override;
	void OnResize(Vec2 newSize) override;

	//Converts a 2D point in the scene image to a 2D point in the real scene
	Vec2 ScreenToWorld(Vec2 p) const;
	//Converts a 2D point in the real scene to a 2D point in the scene image
	Vec2 WorldToScreen(Vec2 p) const;

	static inline const char* GetName() { return "Scene"; };

private:
	//Handles user input
	void HandleInput();
	void HandleGizmoUsage();
public:
	//Gizmo_Op gizmo_op = Gizmo_Op::TRANSLATION //TODO: Used for 2D Gizmos
	//Drag_Type drag = Drag_Type::NONE; //TODO: Used for 2D Gizmos

	Vec2 start_drag;
	Vec2 init_drag_val;


	Vec2 img_corner;
	Vec2 img_size;
	Vec2 cornerPos;
private:
	Vec2 win_size;
	Vec2 img_offset;

	bool draggingOrbit = false;
	bool draggingPan = false;

	//Variables for gizmo handling
	ImGuizmo::OPERATION gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE gizmoMode = ImGuizmo::MODE::WORLD;


};

#endif
