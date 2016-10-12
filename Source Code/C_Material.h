#ifndef __C_MATERIAL_H__
#define __C_MATERIAL_H__

#include "Component.h"
#include <string>

class GameObject;

class C_Material : public Component
{
public:
	C_Material(GameObject* gameObject);
	~C_Material();

	void StackTexture();
	void PopTexture();

public:
	uint texture_id;
	std::string texture_path;
};

#endif // __C_MATERIAL_H__
