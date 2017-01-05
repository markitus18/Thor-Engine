#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "Panel.h"
#include <vector>

class GameObject;
class Component;
class C_Transform;
class C_Mesh;
class C_Camera;
class C_Animation;
class C_Bone;
class R_Material;


class P_Inspector : public Panel
{
public:
	P_Inspector();
	~P_Inspector();

	void Draw(ImGuiWindowFlags flags);

	void DrawGameObject(GameObject* gameObject);

	void DrawTransform(GameObject* gameObject, C_Transform* transform, ImGuiWindowFlags flags);
	void DrawMesh(GameObject* gameObject, C_Mesh* mesh, ImGuiWindowFlags flags);
	void DrawMaterial(GameObject* gameObject, R_Material* material, ImGuiWindowFlags flags);
	void DrawCamera(GameObject* gameObject, C_Camera* camera, ImGuiWindowFlags flags);
	void DrawAnimation(GameObject* gameObject, C_Animation* animation, ImGuiWindowFlags flags);
	void DrawBone(GameObject* gameObject, C_Bone* bone, ImGuiWindowFlags flags);

	void UpdatePosition(int, int);
};

#endif //__PANEL_INSPECTOR_H__
