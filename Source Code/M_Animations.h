#ifndef __M_ANIMATIONS_H__
#define __M_ANIMATIONS_H__

#include "Module.h"
#include "R_Animation.h"

#include <string>
#include <map>

class R_Animation;
class GameObject;
struct aiScene;
struct aiAnimation;
struct aiNodeAnim;
struct aiMesh;
struct aiBone;

class M_Animations : public Module
{
public:
	M_Animations(bool start_enabled = true);
	~M_Animations();

	//Animation data management
	uint64 ImportSceneAnimations(const aiScene* scene, GameObject* root, const char* source_file);
	R_Animation* ImportAnimation(const aiAnimation* animation, uint64 ID, const char* source_file);
	bool SaveAnimationResource(R_Animation* animation);
	R_Animation* LoadAnimation(uint64 ID);

	//Bone data management
	void ImportSceneBones(const std::vector<const aiMesh*>& bonedMeshes, GameObject* root);
	R_Bone* ImportBone(const aiBone*, uint64 ID, const char* source_file);
	bool SaveBoneResource(R_Bone* bone);
	R_Bone* LoadBone(uint64 ID);
	
	//Save utils
	uint CalcChannelSize(const Channel& channel) const;

	void SaveChannelData(const Channel& channel, char* cursor);
	void SaveKeys(const std::map<double, float3>&, char* cursor);
	void SaveKeys(const std::map<double, Quat>&, char* cursor);

	//Load utils
	void LoadChannelData(Channel& channel, char* cursor);
	void LoadKeys(std::map<double, float3>&, char* cursor, uint size);
	void LoadKeys(std::map<double, Quat>&, char* cursor, uint size);

	bool Init(Config& config);
	bool CleanUp();

private:
	void CollectGameObjectNames(GameObject* gameObject, std::map<std::string, GameObject*>& map);
	void LoadChannel(const aiNodeAnim* node, Channel& channel);
};

#endif