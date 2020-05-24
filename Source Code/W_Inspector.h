#ifndef __W_INSPECTOR_H__
#define __W_INSPECTOR_H__

#include "WDetails.h"

#include <vector>
#include <string>

class GameObject;
class C_Transform;
class C_Mesh;
class C_Camera;
class C_Animator;
class C_Billboard;
class C_ParticleSystem;
class C_Material;

class W_Inspector : public WDetails
{
public:
	W_Inspector(M_Editor* editor);
	~W_Inspector() {}

	void Draw();
	void OnResize();

private:
	void DrawGameObject(GameObject* gameObject);

	void DrawTransform(GameObject* gameObject, C_Transform* transform);
	void DrawMesh(GameObject* gameObject, C_Mesh* mesh);
	void DrawMaterials(GameObject* gameObject);
	void DrawMaterial(GameObject* gameObject, C_Material* material, uint index);
	void DrawCamera(GameObject* gameObject, C_Camera* camera);
	void DrawAnimator(GameObject* gameObject, C_Animator* animator);
	void DrawBillboard(GameObject* gameObject, C_Billboard* billboard);
	void DrawParticleSystem(GameObject* gameObject, C_ParticleSystem* particleSystem);

private:
	std::vector<char*> billboardAlignmentOptions;
	std::vector<char*> billboardLockOptions;

	float nameTabRatio = 0.3f;

};

#endif // !__W_INSPECTOR_H__

