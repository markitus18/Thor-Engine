#ifndef __C_MATERIAL_H__
#define __C_MATERIAL_H__

#include "Component.h"
#include <string>
#include "Color.h"

class GameObject;
class R_Material;

class C_Material : public Component
{
public:
	C_Material(GameObject* gameObject);
	~C_Material();

	void Serialize(Config& config) override;

	static inline Type GetType() { return Type::Material; };

	void SetResource(Resource* resource);
	void SetResource(unsigned long long id);

	uint64 GetResourceID() const;

public:
	ResourceHandle<R_Material> rMaterialHandle;
};

#endif // __C_MATERIAL_H__
