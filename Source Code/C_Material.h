#ifndef __C_MATERIAL_H__
#define __C_MATERIAL_H__

#include "Component.h"
#include <string>
#include "Color.h"

class GameObject;

class C_Material : public Component
{
public:
	C_Material(const GameObject* gameObject);
	~C_Material();

	void StackTexture();
	void PopTexture();

	static Component::Type GetType();

public:
	uint texture_id;
	std::string texture_path;
	std::string texture_file;
	Color color;
};

#endif // __C_MATERIAL_H__
