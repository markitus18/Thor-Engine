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
class R_Model;
class R_Map;
struct ModelNode;
class Component;
class GameObject;

class Config;

namespace Importer
{
	namespace Models
	{
		//Creates an empty model resource using default constructor
		R_Model* Create();

		//Processes an already loaded FBX file and loads it into an assimp scene structure
		//Warning: (TODO) do we need to release aiScene data later? (aiReleaseImport is not specificed in Assimp)
		const aiScene* ProcessAssimpScene(const char* buffer, uint size);

		//Processes an already loaded FBX file and generates all the hierarchy and components setup
		//Warning: meshes, materials and lights need to be linked later, this function only loads the hierarchy
		//GameObjects that are meant to have a mesh, material or light, are added the component and given the id from the aiScene container.
		void Import(const aiScene* scene, R_Model* model);

		//'Import' will store all meshes and materials as their index in the scene lists
		//Here we update them to assign the correct resource IDs
		void LinkModelResources(R_Model* model, const std::vector<uint64>& meshes, const std::vector<uint64>& materials);

		//Save all model data (all contained nodes) into a buffer file saved as json
		//Returns the size of the buffer file (0 if any errors)
		//Warning: buffer memory needs to be released after the function call
		uint64 Save(const R_Model* model, char** buffer);

		//Process an json model buffer and loads all the Model nodes
		void Load(const char* buffer, R_Model* model);

		//Process an json model buffer and loads all the GameObject hierarchy
		GameObject* LoadNewRoot(const R_Model* model);

		namespace Private
		{
			//Processes an aiNode structure and generates the GameObject hierarchy recursively.
			//Warning: meshes, materials and lights need to be linked later, this function only loads the hierarchy
			//GameObjects that are meant to have a mesh, material or light, are added the component and given the id from the aiScene container.
			void ImportNodeData(const aiScene* scene, const aiNode* node, R_Model* model, uint64 parentID);

			//Save the info from a model node (name, id, transform, texture and material) into a file
			void SaveModelNode(Config& config, const ModelNode& node);
		}

	}

	namespace Maps
	{
		//Creates an empty scene resource using default constructor
		R_Map* Create();

		//Stores R_Map json config into a file buffer
		uint64 Save(const R_Map* scene, char** buffer);
			   
		//Generates R_Map json config from a file buffer
		void Load(const char* buffer, R_Map* scene);

		//Process a GameObject data with its hierarchy into a json config in R_Map
		void SaveRootToMap(const GameObject* root, R_Map* map);

		//Generates a new root which holds all GameObject hierarchy loaded from R_Map
		GameObject* LoadRootFromMap(const R_Map* map);

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