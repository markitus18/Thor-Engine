#include "M_SceneManager.h"

#include "Engine.h"
#include "Globals.h"
#include "Config.h"
#include "Time.h"

#include "M_Input.h" //TODO: Temporal usage for testing purposes
#include "I_Scenes.h"
#include "M_Resources.h"

#include "GameObject.h"

#include "R_Map.h"
#include "Scene.h"
#include "R_Model.h"

#include <windows.h>
#include <shobjidl.h> 

M_SceneManager::M_SceneManager(bool start_enabled) : Module("Scene", start_enabled)
{

}

M_SceneManager::~M_SceneManager()
{
}


// Load assets
bool M_SceneManager::CleanUp()
{
	LOG("Unloading scene manager");

	return true;
}

GameObject* M_SceneManager::GetRoot()
{
	return gameScene->root;
}

const GameObject* M_SceneManager::GetRoot() const
{
	return gameScene->root;
}

// Update
update_status M_SceneManager::Update()
{
#pragma region WindowTest
	if (Engine->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN)
	{

		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
			COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileOpenDialog *pFileOpen;
			// Create the FileOpenDialog object.
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
				IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

			if (SUCCEEDED(hr))
			{
				// Show the Open dialog box.
				hr = pFileOpen->Show(NULL);
			}
		}
	}
#pragma endregion

	for (uint i = 0; i < activeScenes.size(); ++i)
	{
		activeScenes[i]->UpdateAllGameObjects(Time::deltaTime);
		//TODO: Might not be the best place to call it. Final draw, however, is in render's postupdate
		//if (culling)
			//activeScenes[i]->DrawCulledGameObjects(activeScenes[i]->mainCamera);
		//else
			activeScenes[i]->DrawAllChildren(activeScenes[i]->root);
	}

	return UPDATE_CONTINUE;
}

update_status M_SceneManager::PostUpdate()
{
	for (uint i = 0; i < activeScenes.size(); ++i)
		activeScenes[i]->ClearGameObjectsForRemoval();

	return UPDATE_CONTINUE;
}

void M_SceneManager::SaveConfig(Config& config) const
{

}

void M_SceneManager::LoadConfig(Config& config)
{

}

uint64 M_SceneManager::LoadScene(const char* file, bool append = false)
{
	uint64 newID = Engine->moduleResources->FindResourceBase(file)->ID;
	LoadScene(newID);
	return newID;
}

void M_SceneManager::LoadScene(uint64 resourceID, bool append = false)
{
	if (resourceID != 0)
	{
		if (!append)
		{
			gameScene->ClearScene();
		}

		ResourceHandle<R_Map> rMapHandle(resourceID);
		GameObject* root = Importer::Maps::LoadRootFromMap(rMapHandle.Get());

		std::vector<GameObject*> newGameObjects;
		GetRoot()->CollectChilds(newGameObjects);

		for (uint i = 0; i < newGameObjects.size(); ++i)
			gameScene->AddGameObject(newGameObjects[i]);

		RELEASE(root);
	}
}

void M_SceneManager::LoadModel(uint64 modelID)
{
	//TODO: Almost the same as LoadScene. Can we compress the structure?
	ResourceHandle<R_Model> rModelHandle(modelID);
	GameObject* root = Importer::Models::LoadNewRoot(rModelHandle.Get());

	std::vector<GameObject*> newGameObjects;
	GetRoot()->CollectChilds(newGameObjects);

	for (uint i = 0; i < newGameObjects.size(); ++i)
		gameScene->AddGameObject(newGameObjects[i]);
	
	RELEASE(root);
}

void M_SceneManager::StartSceneSimulation(Scene* scene)
{
	//TODO: Call resources and save current scene ("Engine/tmp.scene")
	//Only for current game scene? A specific file for each scene...?
	scene->Play();
}

void M_SceneManager::StopSceneSimulation(Scene* scene)
{
	//TODO: Call resources to load scene previous to play ("Engine/tmp.scene")
	//Only for current game scene? A specific file for each scene...?
	scene->Stop();
}

void M_SceneManager::PauseSceneSimulation(Scene* scene)
{

}

void M_SceneManager::RemoveScene(Scene* scene)
{
	if (scene == gameScene)
	{
		LOG("[Error]: Trying to remove current game scene");
		return;
	}

	std::vector<Scene*>::iterator it;
	for (it = activeScenes.begin(); it != activeScenes.end(); ++it)
	{
		if ((*it) == scene)
		{
			activeScenes.erase(it);
			return;
		}
	}
}