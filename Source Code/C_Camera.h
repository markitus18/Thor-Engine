#ifndef  __CAMERA_H__
#define __CAMERA_H__

#include "Component.h"

#include "MathGeoLib\src\MathBuildConfig.h"
#include "MathGeoLib\src\MathGeoLib.h"

class C_Camera : public Component
{
public:
	C_Camera();
	~C_Camera();

private:
	Frustum frustrum;
};

#endif // __CAMERA_H__

