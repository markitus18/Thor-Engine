#include "Scene.h"

#include "Engine.h"
#include "M_Editor.h"

#include "WViewport.h" //There should be a more polite way than accessing viewports from scene
#include "M_Renderer3D.h"

#include "Time.h"

#include "GameObject.h"
#include "Quadtree.h"
#include "ResourceBase.h"
#include "C_Transform.h"
#include "C_Camera.h"
#include "C_Mesh.h"
#include "R_Mesh.h"

Scene::Scene()
{
	quadtree = new Quadtree(AABB(vec(-80, -30, -80), vec(80, 30, 80)));
	root = new GameObject(nullptr, float4x4::identity, "Root");
}

Scene::~Scene()
{

}

void Scene::InitFromResourceData(const ResourceBase* data)
{
	ID = data->ID;
	file_path = data->assetsFile;
	name = data->name;
}

void Scene::AddGameObject(GameObject* newGameObject, GameObject* parent)
{
	newGameObject->SetParent(parent ? parent : root);

	std::vector<GameObject*> newGameObjects;
	newGameObject->CollectChilds(newGameObjects);

	for (uint i = 0; i < newGameObjects.size(); ++i)
	{
		newGameObjects[i]->sceneOwner = this;
		OnGameObjectAdded(newGameObjects[i]);
		OnGameObjectStaticChanged(newGameObjects[i]);
	}
}

GameObject* Scene::CreateNewGameObject(const char* name, GameObject* parent)
{
	GameObject* go = new GameObject(parent ? parent : root, name);
	go->uid = random.Int();
	go->sceneOwner = this;
	return go;
}

std::string Scene::GetNewGameObjectName(const char* name, const GameObject* parent)
{
	uint count = GetGameObjectNameCount(name, parent);
	std::string newName = name;
	if (count > 0)
		newName.append(" (").append(std::to_string(count)).append(")");
	return newName;
}

int Scene::GetGameObjectNameCount(const char* name, const GameObject* parent)
{
	if (parent == nullptr) parent = root;
	if (parent->childs.size() == 0) return 0;

	uint count = 0;
	bool nameMatch = true;

	while (nameMatch)
	{
		std::string str = name;
		if (count > 0)
			str.append(" (").append(std::to_string(count)).append(")");
		if (nameMatch = parent->FindChildByName(str.c_str()))
			++count;
	}

	return count;
}

void Scene::OnGameObjectAdded(GameObject* gameObject)
{
	// Add GameObject to Space Partitioning
	OnGameObjectStaticChanged(gameObject);

	// Collect GameObject stats
	if (C_Mesh* mesh = gameObject->GetComponent<C_Mesh>())
	{
		++stats.meshCount;
		if (mesh->rMeshHandle.Get())
		{
			stats.triangleCount += mesh->rMeshHandle.Get()->buffersSize[R_Mesh::b_indices] / 3;
		}
	}
	++stats.objectsCount;
}

void Scene::OnDestroyGameObject(GameObject* gameObject)
{
	//Remove from quadtree // non-static vector
	if (quadtree->RemoveGameObject(gameObject) == false)
	{
		std::vector<const GameObject*>::iterator it;
		it = find(dynamicGameObjects.begin(), dynamicGameObjects.end(), gameObject);

		if (it != dynamicGameObjects.end())
			dynamicGameObjects.erase(it);
	}
	gameObjectsPendingRemoval.push_back(gameObject);

	// Remove GameObject stats
	if (C_Mesh* mesh = gameObject->GetComponent<C_Mesh>())
	{
		--stats.meshCount;
		if (mesh->rMeshHandle.Get())
		{
			stats.triangleCount -= mesh->rMeshHandle.Get()->buffersSize[R_Mesh::b_indices] / 3;
		}
	}
	--stats.objectsCount;

	//TODO: WARNING, temporal setup. Engine should not be accessed from scene
	Engine->OnRemoveGameObject(gameObject);
}

void Scene::OnGameObjectStaticChanged(GameObject* gameObject)
{
	if (gameObject->isStatic)
	{
		quadtree->AddGameObject(gameObject);

		std::vector<const GameObject*>::iterator it;
		it = find(dynamicGameObjects.begin(), dynamicGameObjects.end(), gameObject);

		if (it != dynamicGameObjects.end())
			dynamicGameObjects.erase(it);
	}
	else
	{
		quadtree->RemoveGameObject(gameObject);
		dynamicGameObjects.push_back(gameObject);
	}
}

void Scene::PerformMousePick(const LineSegment& segment)
{
	LOG("Starting Mouse Raycast...");

	// Collect quadtree GameObjects
	std::map<float, const GameObject*> candidates;
	quadtree->CollectCandidates(candidates, segment);

	// Collect dynamic GameObjects
	for (uint i = 0; i < dynamicGameObjects.size(); i++)
	{
		if (segment.Intersects(dynamicGameObjects[i]->GetAABB()))
		{
			float hit_near, hit_far;
			if (segment.Intersects(dynamicGameObjects[i]->GetOBB(), hit_near, hit_far))
			{
				candidates[hit_near] = dynamicGameObjects[i];
			}
		}
	}

	// Iterate all candidates from near to far, check for mesh intersection
	// *We iterate all candidates since MathGeoLib does not give accurate OBB distance results
	const GameObject* finalCandidate = nullptr;
	float finalDistance = segment.Length();

	std::map<float, const GameObject*>::iterator it;
	for (it = candidates.begin(); it != candidates.end(); ++it)
	{
		if (const Component* mesh = it->second->GetComponent<C_Mesh>())
			if (const R_Mesh* rMesh = ((C_Mesh*)mesh)->rMeshHandle.Get())
			{
				//Convert segment in world space coordinates to model space coordinates
				LineSegment rayToMeshSpace = segment;
				rayToMeshSpace.Transform(it->second->GetComponent<C_Transform>()->GetTransform().Inverted());

				//Iterate all mesh triangles until we find a hit
				for (uint v = 0; v < rMesh->buffersSize[R_Mesh::b_indices]; v += 3)
				{
					vec vertexA(&rMesh->vertices[rMesh->indices[v] * 3]);
					vec vertexB(&rMesh->vertices[rMesh->indices[v + 1] * 3]);
					vec vertexC(&rMesh->vertices[rMesh->indices[v + 2] * 3]);

					Triangle triangle(vertexA, vertexB, vertexC);

					float newDistance = 0.0f;
					if (rayToMeshSpace.Intersects(triangle, &newDistance, nullptr))
					{
						if (newDistance < finalDistance)
						{
							finalCandidate = it->second;
							finalDistance = newDistance;
						}
					}
				}
			}
	}

	//TODO: PerformMousePick should return final candidate, which will be grabbed by viewport and then call editor
	Engine->moduleEditor->SelectSingle((GameObject*)finalCandidate);
}

GameObject* Scene::CreateCamera()
{
	GameObject* camera = CreateNewGameObject("Camera", root);
	camera->GetComponent<C_Transform>()->SetLocalPosition(float3(10, 10, 0));
	camera->CreateComponent(Component::Type::Camera);
	camera->GetComponent<C_Transform>()->LookAt(float3(0, 5, 0));
	camera->uid = random.Int();

	//TODO: Keeping a reference to the last camera, by now.
	//Scene settings should be able to select the main game camera
	mainCamera = camera->GetComponent<C_Camera>();
	return camera;
}

const C_Camera* Scene::GetMainCamera() const
{
	return mainCamera;
}

void Scene::RegisterCamera(C_Camera* camera)
{
	registeredCameras.push_back(camera);
}

void Scene::UnregisterCamera(C_Camera* camera)
{
	std::vector<C_Camera*>::iterator it = std::find(registeredCameras.begin(), registeredCameras.end(), camera);
	if (it != registeredCameras.end())
	{
		registeredCameras.erase(it);
	}
}

void Scene::Play()
{
	Time::Start(60);
	//TODO: Call resources and save current scene ("Engine/tmp.scene")
	//It should probably be handled through scene manager
}

void Scene::Stop()
{
	Time::Stop();
	//TODO: Call resources to load scene previous to play ("Engine/tmp.scene")
	//It should probably be handled through scene manager
}


void Scene::ClearScene()
{
	if (!root) return; //Scene has not been initialized yet

	while(root->childs.size() > 0) 
	{
		root->childs[0]->Destroy(); //Is this clean procedure? Feels kind of dirty
	}
}

void Scene::ClearGameObjectsForRemoval()
{
	for (std::vector<GameObject*>::iterator it = gameObjectsPendingRemoval.begin(); it != gameObjectsPendingRemoval.end(); it++)
	{
		RELEASE(*it);
	}
	gameObjectsPendingRemoval.clear();
}

void Scene::Update()
{
	root->Update();
}

void Scene::DrawScene()
{
	std::vector<C_Camera*>::iterator cameraIt;
	for (cameraIt = registeredCameras.begin(); cameraIt != registeredCameras.end(); ++cameraIt)
	{
		Engine->renderer3D->BeginTargetCamera(*cameraIt, (*cameraIt)->viewMode);

		//TODO: Insert camera culling here to avoid unnecessary draw calls
		DrawAllChildren(root, (*cameraIt)->renderingFlags);
		(*cameraIt)->Draw(ERenderingFlags::MousePick & (*cameraIt)->renderingFlags);

		Engine->renderer3D->EndTargetCamera();
	}
}

void Scene::DrawAllChildren(GameObject* gameObject, RenderingFlags flags)
{
	//TODO: Draw quadtree somewhere
	if (gameObject != root)
		gameObject->Draw(flags);

	for (uint i = 0; i < gameObject->childs.size(); ++i)
		DrawAllChildren(gameObject->childs[i], flags);
}

void Scene::DrawCulledGameObjects(C_Camera* targetCamera)
{
	std::vector<const GameObject*> treeCandidates;
//quadtree->CollectCandidates(treeCandidates, targetCamera->frustum);

	std::vector<const GameObject*> finalCandidates;
	CollectCullingCandidates(treeCandidates, finalCandidates);
	CollectCullingCandidates(dynamicGameObjects, finalCandidates);

	for (uint i = 0; i < finalCandidates.size(); i++)
	{
		if (finalCandidates[i]->name != "root")
			((GameObject*)finalCandidates[i])->Draw(0);
	}
}

void Scene::CollectCullingCandidates(std::vector<const GameObject*>& vector, std::vector<const GameObject*>& candidates)
{
	for (uint i = 0; i < vector.size(); i++)
	{
		//TODO: Culling camera should be held in Scene or given as a parameter when calling collect culling candidates
		//if (Engine->renderer3D->culling_camera->frustum.Intersects(vector[i]->GetAABB()))
		//{
		//	candidates.push_back(vector[i]);
		//}
	}
}
