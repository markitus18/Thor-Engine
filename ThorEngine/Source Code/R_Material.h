#ifndef __R_MATERIAL_H__
#define __R_MATERIAL_H__

#include "Globals.h"

#include "Resource.h"
#include "ResourceHandle.h"

#include "Color.h"

class R_Shader;
class R_Texture;

class R_Material : public Resource
{
public:
	R_Material();
	~R_Material();

	void Serialize(Config& config);

public:
	ResourceHandle<R_Shader> hShader;
	ResourceHandle<R_Texture> hTexture;

	Color color;
};

#endif //__R_MATERIAL_H__