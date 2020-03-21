#ifndef __M_SHADERS_H__
#define __M_SHADERS_H__

#include "Globals.h"

class R_Shader;

namespace Importer
{
	namespace Shaders
	{
		//Creates an empty shader resource using default constructor
		R_Shader* Create();

		//Processes GLSL buffer data into a ready-to-use R_Shader to be saved later.
		void Import(const char* buffer, R_Shader* resShader);

		//Process R_Shader data into a buffer ready to save
		//Returns the size of the buffer file (0 if any errors)
		//Warning: buffer memory needs to be released after the function call
		uint64 Save(const R_Shader* resShader, char** buffer);

		//Process buffer data into a ready-to-use R_Mesh.
		//Returns nullptr if any errors occured during the process.
		void Load(const char* buffer, uint size, R_Shader* shader);
	}
}
#endif // !


