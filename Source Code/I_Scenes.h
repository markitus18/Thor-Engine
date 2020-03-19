#ifndef __MODULE_IMPORT_H__
#define __MODULE_IMPORT_H__

#include "Globals.h"
#include <vector>

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

class C_Mesh;
class C_Material;
class C_Animator;
class C_Camera;
class C_Transform;
class C_ParticleSystem;
class R_Prefab;
class Component;
class GameObject;

class Config;

namespace Importer
{
	namespace Scenes
	{
		void Init();

		//Processes an already loaded FBX file and loads it into an assimp scene structure
		//Warning: (TODO) do we need to release aiScene data later? (aiReleaseImport is not specificed in Assimp)
		const aiScene* ProcessAssimpScene(const char* buffer, uint size);

		//Processes an already loaded FBX file and generates all the hierarchy and components setup
		//Warning: meshes, materials and lights need to be linked later, this function only loads the hierarchy
		//GameObjects that are meant to have a mesh, material or light, are added the component and given the id from the aiScene container.
		GameObject* Import(const aiScene* scene, const char* name);

		//Processes an aiNode structure and generates the GameObject hierarchy recursively.
		//Warning: meshes, materials and lights need to be linked later, this function only loads the hierarchy
		//GameObjects that are meant to have a mesh, material or light, are added the component and given the id from the aiScene container.
		GameObject* CreateGameObjectFromNode(const aiScene* scene, const aiNode* node, GameObject* parent);

		void LinkSceneResources(GameObject* gameObject, const std::vector<uint64>& meshes, const std::vector<uint64>& materials);

		//Process a GameObject data with its hierarchy into a buffer file saved as json
		//Returns the size of the buffer file (0 if any errors)
		//Warning: buffer memory needs to be released after the function call
		uint64 SaveScene(const R_Prefab* prefab, char** buffer);

		//Process an json scene buffer and loads all the GameObject hierarchy
		void LoadScene(const Config& file, std::vector<GameObject*>& roots);

		//Saves the contained resources in a prefab into a Config file
		void SaveContainedResources(R_Prefab* prefab, Config& file);

		//Loads the contained resources from a prefab meta file into the prefab resources
		void LoadContainedResources(const Config& file, R_Prefab* prefab);

		namespace Private
		{
			//Process a GameObject data with its hierarchy into a config file
			//This function will be called recursively for every child in <gameObject>
			void SaveGameObject(Config& config, const GameObject* gameObject);

			//Process a Component base data into a config file
			//This function will call specific functions for each component type
			void SaveComponentBase(Config& config, const Component* component);

			//Select the specific component class to be saved and calls its according function
			void SaveComponent(Config& config, const Component* component);

			//Process a Camera component data into a config file
			void SaveComponent(Config& config, const C_Camera* component);

			//Process an Animation component data into a config file
			void SaveComponent(Config& config, const C_Animator* component);

			//Process a Particle System component data into a config file
			void SaveComponent(Config& config, const C_ParticleSystem* component);

			//Loads the base for all components and calls the specific component load function
			void LoadComponent(Config& config, Component* component);

			void LoadComponent(Config& config, C_Camera* camera);

			void LoadComponent(Config& config, C_Animator* animation);

			void LoadComponent(Config& config, C_ParticleSystem* particleSystem);
		}
	}
}

#endif