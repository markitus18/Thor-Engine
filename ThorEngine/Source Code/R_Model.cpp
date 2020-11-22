#include "R_Model.h"

R_Model::R_Model() : Resource(ResourceType::MODEL)
{
	isExternal = true;
}

R_Model::~R_Model()
{

}

void R_Model::Serialize(Config& config)
{
	config.SerializeVector<ModelNode>("Nodes", nodes);
}