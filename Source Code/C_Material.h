#ifndef __C_MATERIAL_H__
#define __C_MATERIAL_H__

#include "Component.h"
#include <string>
#include "Color.h"

class GameObject;

class C_Material : public Component
{
public:
	C_Material(GameObject* gameObject);
	~C_Material();

	static inline Type GetType() { return Type::Material; };
};

#endif // __C_MATERIAL_H__
