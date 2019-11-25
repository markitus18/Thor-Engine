#ifndef __C_BILLBOARD_H__
#define __C_BILLBOARD_H__

#include "Component.h"

class C_Billboard : public Component
{
public:
	C_Billboard(GameObject* gameObject);
	~C_Billboard();

	static inline Type GetType() { return Type::Billboard; };
};

#endif // !
