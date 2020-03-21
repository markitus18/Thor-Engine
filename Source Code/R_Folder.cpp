#include "R_Folder.h"

R_Folder::R_Folder() : Resource(Resource::Type::FOLDER)
{
	isInternal = true;
}

R_Folder::~R_Folder()
{

}