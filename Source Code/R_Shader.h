#ifndef __R_SHADER_H__
#define __R_SHADER_H__

#include "Resource.h"

class R_Shader : public Resource
{
public:

	enum class Object_Type
	{
		Vertex,
		Fragment,
		Unknown
	};

	R_Shader();
	~R_Shader();

	uint Save(char** buffer) const;
	bool LoadFromText(const char* buffer);
	bool LoadFromBinary(const char* buffer, int size);
	bool Link();

	void DeleteShaderObject(int index);

	static std::string GetShaderMacroStr(Object_Type type);
	static int GetShaderMacro(Object_Type type);

public:
	uint64 shaderProgram = 0;
	uint64 shaderObjects[(int)Object_Type::Unknown];
};
#endif
