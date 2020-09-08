#include "ResourceHandle.h"

#include "Engine.h"
#include "M_Resources.h"

#include "R_Mesh.h"
#include "R_Model.h"
#include "R_Scene.h"
#include "R_Shader.h"
#include "R_Folder.h"
#include "R_Texture.h"
#include "R_Material.h"
#include "R_Animation.h"
#include "R_ParticleSystem.h"
#include "R_AnimatorController.h"

template class ResourceHandle<Resource>;
template class ResourceHandle<R_Mesh>;
template class ResourceHandle<R_Model>;
template class ResourceHandle<R_Scene>;
template class ResourceHandle<R_Folder>;
template class ResourceHandle<R_Shader>;
template class ResourceHandle<R_Texture>;
template class ResourceHandle<R_Material>;
template class ResourceHandle<R_ParticleSystem>;
template class ResourceHandle<R_AnimatorController>;

template <typename T>
T* ResourceHandle<T>::RequestResource() const
{
	return dynamic_cast<T*>(Engine->moduleResources->RequestResource(ID));
}

template <typename T>
void ResourceHandle<T>::Free()
{
	if (resource != nullptr)
	{
		Engine->moduleResources->ReleaseResource(resource);
		resource = nullptr;
	}
}


