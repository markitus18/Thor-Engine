#ifndef __I_FOLDERS_H__
#define __I_FOLDERS_H__

#include "Globals.h"

class R_Folder;

namespace Importer
{
	namespace Folders
	{
		R_Folder* Create();

		uint64 Save(const R_Folder* rFolder, char** buffer);
		void Load(const char* buffer, R_Folder* rAnimation);
	}
}
#endif