#include "R_Folder.h"

R_Folder::R_Folder() : Resource(Resource::Type::FOLDER)
{

}

R_Folder::~R_Folder()
{

}

const std::vector<Resource*>& R_Folder::GetChilds() const
{
	return childs;
}