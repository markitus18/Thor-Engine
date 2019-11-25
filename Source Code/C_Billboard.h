#ifndef __C_BILLBOARD_H__
#define __C_BILLBOARD_H__

#include "Component.h"

class C_Camera;
class C_Transform;

class C_Billboard : public Component
{
public:
	enum class Alignment
	{
		Screen_Aligned,
		Camera_Aligned,
		Axis_Aligned,
		Unknown
	};

	enum class Axis
	{
		x,
		y,
		z,
		Unknown
	};

	C_Billboard(GameObject* gameObject);
	~C_Billboard();

	static inline Type GetType() { return Type::Billboard; };

	void Update(float dt);

private:
	void GetVectorsFromAlignment(const C_Transform& transform, const C_Transform& cameraTransform, float3&right, float3&up, float3& fwd) const;

public:
	Alignment alignment = Alignment::Camera_Aligned;
	Axis lockAxis = Axis::y;

private:
	//Keeping a reference to the camera for faster iterations
	const C_Camera* cameraRef = nullptr;
};

#endif // !
