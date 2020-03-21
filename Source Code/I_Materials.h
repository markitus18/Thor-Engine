#ifndef __MODULE_MATERIALS_H__
#define __MODULE_MATERIALS_H__

#include "Globals.h"

class C_Material;
struct aiMaterial;
class R_Texture;
class R_Material;

namespace Importer
{
	namespace Materials
	{
		//Creates an empty mesh resource using default constructor
		R_Material* Create();

		//Processes aiMaterial data into a ready-to-use R_Material to be saved later.
		//Returns nullptr if any errors occured during the process.
		void Import(const aiMaterial* material, R_Material* rMaterial);

		//Process R_Material data into a buffer ready to save
		//Returns the size of the buffer file (0 if any errors)
		//Warning: buffer memory needs to be released after the function call
		uint64 Save(const R_Material* rMaterial, char** buffer);

		//Process buffer data into a ready-to-use R_Material.
		//Returns nullptr if any errors occured during the process.
		void Load(const char* buffer, uint size, R_Material* rMaterial);
	}

	namespace Textures
	{
		//Initializes the library used to handle texture image importing
		void Init();

		//Creates an empty texture resource using default constructor
		R_Texture* Create();

		//Processes buffer data into a ready-to-use R_Texture to be saved later.
		//Returns nullptr if any errors occured during the process.
		bool Import(const char* buffer, uint size, R_Texture* rTexture);

		//This function is just to keep consistency in all importers. Save NEEDS to be called after
		//Import due to DevIL memory management
		uint64 Save(const R_Texture* rTexture, char** buffer);

		//Process buffer data into a ready-to-use R_Mesh.
		//Returns nullptr if any errors occured during the process.
		void Load(const char* buffer, uint size, R_Texture* texture);
	}
}

#endif // __MODULE_MATERIALS_H__
