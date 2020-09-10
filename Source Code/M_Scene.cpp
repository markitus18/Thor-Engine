#include "M_Scene.h"

#include "Engine.h"
#include "Globals.h"
#include "Intersections.h"
#include "Config.h"
#include "Quadtree.h"
#include "Time.h"

#include "M_Camera3D.h"
#include "M_Input.h"
#include "I_Scenes.h"
#include "M_Editor.h"
#include "M_Renderer3D.h"
#include "M_FileSystem.h"
#include "M_Resources.h"

#include "GameObject.h"

#include "R_Scene.h"
#include "R_Model.h"
#include "R_Mesh.h"

#include "C_Mesh.h"
#include "C_Transform.h"
#include "C_Camera.h"

#include <windows.h>
#include <shobjidl.h> 

M_Scene::M_Scene(bool start_enabled) : Module("Scene", start_enabled)
{

}

M_Scene::~M_Scene()
{
}

bool M_Scene::Init(Config& config)
{
	quadtree = new Quadtree(AABB(vec(-80, -30, -80), vec(80, 30, 80)));

	return true;
}

// Load assets
bool M_Scene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	return ret;
}

// Load assets
bool M_Scene::CleanUp()
{
	LOG("Unloading scene");

	return true;
}

GameObject* M_Scene::GetRoot()
{
	return hScene.Get()->root;
}

const GameObject* M_Scene::GetRoot() const
{
	return hScene.Get()->root;
}

// Update
update_status M_Scene::Update()
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
 	UpdateAllGameObjects(GetRoot(), Time::deltaTime);

	if (Engine->renderer3D->culling_camera)
	{
		std::vector<const GameObject*> candidates;

		quadtree->CollectCandidates(candidates, Engine->renderer3D->culling_camera->frustum);

		std::vector<const GameObject*> gameObjects;
		TestGameObjectsCulling(candidates, gameObjects);
		TestGameObjectsCulling(nonStatic, gameObjects);

		for (uint i = 0; i < gameObjects.size(); i++)
		{
			if (gameObjects[i]->name != "root");
			((GameObject*)gameObjects[i])->Draw(true, false, drawBounds, drawBoundsSelected);
		}
		gameObjects.clear();
	}
	else
	{
		DrawAllGameObjects(GetRoot());
	}

	if (drawQuadtree)
		quadtree->Draw();

	return UPDATE_CONTINUE;
}

update_status M_Scene::PostUpdate()
{
	DeleteToRemoveGameObjects();
	return UPDATE_CONTINUE;
}

std::string M_Scene::GetNewGameObjectName(const char* name, const GameObject* parent) const
{
	uint count = GetGameObjectNameCount(name, parent);
	std::string newName = name;
	if (count > 0)
		newName.append(" (").append(std::to_string(count)).append(")");
	return newName;
}

int M_Scene::GetGameObjectNameCount(const char* name, const GameObject* parent) const
{
	if (parent == nullptr) parent = GetRoot();
	if (parent->childs.size() == 0) return 0;

	std::vector<GameObject*>::iterator it;
	uint count = 0;
	bool found = false;

	do
	{
		std::string nameStr = name;
		if (count > 0)
		{
			nameStr.append(" (").append(std::to_string(count)).append(")");
		}
		if (found = parent->FindChildByName(nameStr.c_str()))
		{
			++count;
		}
	} while (found == true);
	return count;
}

void M_Scene::SaveConfig(Config& config) const
{

}

void M_Scene::SetStaticGameObject(GameObject* gameObject, bool isStatic, bool allChilds)
{
	if (gameObject->isStatic != isStatic)
	{
		gameObject->SetStatic(isStatic);

		if (isStatic == true)
		{
			GameObject* it = gameObject->parent;
			while (it != nullptr && it->name != "root")
			{
				SetStaticGameObject(it, isStatic, false);
				it = it->parent;
			}

			quadtree->AddGameObject(gameObject);

			for (std::vector<const GameObject*>::iterator it = nonStatic.begin(); it != nonStatic.end(); it++)
			{
				if ((*it) == gameObject)
				{
					nonStatic.erase(it);
					break;
				}
			}
		}
		else
		{
			quadtree->RemoveGameObject(gameObject);
			nonStatic.push_back(gameObject);
		}
	}

	if (allChilds)
	{
		for (uint i = 0; i < gameObject->childs.size(); i++)
		{
			SetStaticGameObject(gameObject->childs[i], isStatic, allChilds);
		}
	}
}

void M_Scene::LoadConfig(Config& config)
{

}

uint64 M_Scene::LoadScene(const char* file)
{
	uint64 newID = Engine->moduleResources->FindResourceBase(file)->ID;

	if (newID != 0)
	{
		DeleteAllGameObjects();
		quadtree->Clear();
		hScene.Set(newID); hScene.Get(); //<-- So the resource gets loaded
		std::vector<GameObject*> newGameObjects;
		GetRoot()->CollectChilds(newGameObjects);

		for (uint i = 0; i < newGameObjects.size(); i++)
		{
			if (newGameObjects[i]->isStatic)
			{
				quadtree->AddGameObject(newGameObjects[i]);
			}
			else
			{
				nonStatic.push_back(newGameObjects[i]);
			}
		}
	}
	return hScene.GetID();
}

void M_Scene::LoadModel(uint64 modelID)
{
	std::vector<GameObject*> newGameObjects;
	//Resource handle will be deleted at the end of the function and the resource will be freed
	ResourceHandle<R_Model> rModelHandle(modelID);
	R_Model* model = rModelHandle.Get();

	if (model != nullptr)
	{
		//Port each model children into the current scene
		model->root->SetParent(GetRoot());

		//Add all gameObject's children to the static vector
		model->root->CollectChilds(newGameObjects);
		for (uint i = 0; i < newGameObjects.size(); i++)
		{
			nonStatic.push_back(newGameObjects[i]);
		}
		newGameObjects.clear();
	}
	else
	{
		LOG("[error] File '%s' could not be loaded");
	}
}

GameObject* M_Scene::CreateGameObject(const char* name, GameObject* parent)
{
	GameObject* go = new GameObject(parent ? parent : GetRoot(), name);
	go->uid = random.Int();
	return go;
}

void M_Scene::DeleteGameObject(GameObject* gameObject)
{
	bool add = true;
	for (uint i = 0; i < toRemove.size(); i++)
	{
		if (toRemove[i] == gameObject)
			return;
	}
	toRemove.push_back(gameObject);
	Engine->OnRemoveGameObject(gameObject);

	while (gameObject->childs.empty() == false) //"DeleteGameObject" will remove the gameObject from the childs list
	{
		DeleteGameObject(gameObject->childs[0]);
	}
}

void M_Scene::OnRemoveGameObject(GameObject* gameObject)
{
	//Remove from quadtree // non-static vector
	if (quadtree->RemoveGameObject(gameObject) == false)
	{
		bool erased = false;
		for (std::vector<const GameObject*>::iterator it = nonStatic.begin(); it != nonStatic.end(); it++)
		{
			if (*it == gameObject)
			{
				nonStatic.erase(it);
				erased = true;
				break;
			}
		}
		if (erased == false)
			LOG("[warning] deleted GameObject not found in quadtree nor non-static vector");
	}

	//Removing parent child
	if (gameObject->parent)
	{
		GameObject* parent = gameObject->parent;
		for (std::vector<GameObject*>::iterator it = parent->childs.begin(); it != parent->childs.end();)
		{
			*it == gameObject ? it = parent->childs.erase(it) : it++;
		}
	}
}

void M_Scene::OnClickSelection(const LineSegment& segment)
{
	//Collecting quadtree GameObjects
	std::map<float, const GameObject*> candidates;
	quadtree->CollectCandidates(candidates, segment);

	//Collecting non-static GameObjects
	for (uint i = 0; i < nonStatic.size(); i++)
	{
		if (segment.Intersects(nonStatic[i]->GetAABB()))
		{
			float hit_near, hit_far;
			if (segment.Intersects(nonStatic[i]->GetOBB(), hit_near, hit_far))
				candidates[hit_near] = nonStatic[i];
		}
	}


	const GameObject* toSelect = nullptr;
	for (std::map<float, const GameObject*>::iterator it = candidates.begin(); it != candidates.end() && toSelect == nullptr; it++)
	{
		//Testing triangle by triangle
		const Component* mesh = it->second->GetComponent<C_Mesh>();
		if (mesh)
		{
			const R_Mesh* rMesh = ((C_Mesh*)mesh)->rMeshHandle.Get();

			if (rMesh)
			{
				LineSegment local = segment;
				local.Transform(it->second->GetComponent<C_Transform>()->GetGlobalTransform().Inverted());
				for (uint v = 0; v < rMesh->buffersSize[R_Mesh::b_indices]; v += 3)
				{
					uint indexA = rMesh->indices[v] * 3;
					vec a(&rMesh->vertices[indexA]);

					uint indexB = rMesh->indices[v + 1] * 3;
					vec b(&rMesh->vertices[indexB]);

					uint indexC = rMesh->indices[v + 2] * 3;
					vec c(&rMesh->vertices[indexC]);

					Triangle triangle(a, b, c);

					if (local.Intersects(triangle, nullptr, nullptr))
					{
						toSelect = it->second;
						break;
					}
				}
			}
		}
	}
	Engine->moduleEditor->SelectSingle((GameObject*)toSelect);
}

GameObject* M_Scene::CreateCamera()
{
	GameObject* camera = new GameObject(GetRoot(), "Camera");
	camera->GetComponent<C_Transform>()->SetPosition(float3(10, 10, 0));
	camera->CreateComponent(Component::Type::Camera);
	camera->GetComponent<C_Camera>()->Look(float3(0, 5, 0));
	camera->uid = random.Int();

	//Keeping a reference to the last camera, by now
	hScene.Get()->mainCamera = camera->GetComponent<C_Camera>();
	return camera;
}

const C_Camera* M_Scene::GetMainCamera() const
{
	return hScene.Get()->mainCamera;
}

void M_Scene::Play()
{
	Time::Start(60);
	Engine->moduleResources->SaveResourceAs(hScene.Get(), "Engine", "tmp.scene");
}

void M_Scene::Stop()
{
	Time::Stop();

	R_Scene* realScene = hScene.Get();
	realScene->instances++; //Increasing instances just in case. LoadScene will release one instance as the resource is changed
	LoadScene("Engine/tmp.scene");
	realScene->root = hScene.Get()->root;
	hScene.Get()->root = nullptr;
	hScene.Set(realScene);
}

void M_Scene::TestGameObjectsCulling(std::vector<const GameObject*>& vector, std::vector<const GameObject*>& final)
{
	for (uint i = 0; i < vector.size(); i++)
	{
		if (Engine->renderer3D->culling_camera->frustum.Intersects(vector[i]->GetAABB()))
		{
			final.push_back(vector[i]);
		}
	}
}

void M_Scene::UpdateAllGameObjects(GameObject* gameObject, float dt)
{
	GetRoot()->Update(dt);
}

void M_Scene::DrawAllGameObjects(GameObject* gameObject)
{
	if (gameObject->name != "root");
		gameObject->Draw(true, false, drawBounds, drawBoundsSelected);

	for (uint i = 0; i < gameObject->childs.size(); i++)
	{
		DrawAllGameObjects(gameObject->childs[i]);
	}
}

void M_Scene::FindGameObjectByID(uint id, GameObject* gameObject, GameObject** ret)
{
	if (gameObject->uid == id)
	{
		if (*ret != nullptr)
			LOG("[error] Conflict: GameObjects with same UID");
		*ret = gameObject;
	}

	//Optimization vs security: if ret != nullptr we can stop searching
	//If we search for all game objects, we can find uid conflicts if any
	for (uint i = 0; i < gameObject->childs.size(); i++)
	{
		FindGameObjectByID(id, gameObject->childs[i], ret);
	}

}

void M_Scene::DeleteAllGameObjects()
{
	if (hScene.GetID())
		RELEASE(hScene.Get()->root);
}

void M_Scene::DeleteToRemoveGameObjects()
{
	for (std::vector<GameObject*>::iterator it = toRemove.begin(); it != toRemove.end(); it++)
	{
		RELEASE(*it);
	}
	toRemove.clear();
}