#include "W_Inspector.h"

#include "ImGui/imgui.h"

#include "M_Editor.h"
#include "Engine.h"
#include "M_SceneManager.h" //Included for static changes
#include "M_Resources.h"
#include "M_Camera3D.h" //Included for camera editor matching
#include "M_Renderer3D.h" //Included for camera editor matching
#include "W_Explorer.h"

#include "WF_SceneEditor.h"

#include "TreeNode.h"
#include "GameObject.h"

#include "C_Mesh.h"
#include "C_Transform.h"
#include "C_Material.h"
#include "C_Animator.h"
#include "C_Camera.h"
#include "C_Billboard.h"
#include "C_ParticleSystem.h"

#include "R_Mesh.h"
#include "R_Material.h"
#include "R_Texture.h"
#include "R_Animation.h"
#include "R_AnimatorController.h"
#include "R_ParticleSystem.h"

W_Inspector::W_Inspector(M_Editor* editor, ImGuiWindowClass* windowClass, int ID) : WDetails(editor, GetName(), windowClass, ID)
{
	billboardAlignmentOptions.push_back("Screen");
	billboardAlignmentOptions.push_back("Camera");
	billboardAlignmentOptions.push_back("Axis");

	billboardLockOptions.push_back("x");
	billboardLockOptions.push_back("y");
	billboardLockOptions.push_back("z");
}

void W_Inspector::Draw()
{
	ImGui::SetNextWindowClass(windowClass);
	if (!ImGui::Begin(windowStrID.c_str(), &active)) { ImGui::End(); return; }

	if (editor->selectedGameObjects.size() > 0)
	{
		DrawGameObject((GameObject*)editor->selectedGameObjects[0]);
	}

	ImGui::End();

}

void W_Inspector::DrawGameObject(GameObject* gameObject)
{
	ImGui::Indent();

	//Active button
	ImGui::Checkbox("##", &gameObject->active);
	ImGui::SameLine();

	//Name input
	char go_name[50];
	strcpy_s(go_name, 50, gameObject->name.c_str());
	ImGuiInputTextFlags name_input_flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue;
	if (ImGui::InputText("###", go_name, 50, name_input_flags))
		gameObject->name = go_name;
	ImGui::SameLine();
	bool gameObject_static = gameObject->isStatic;
	if (ImGui::Checkbox("static", &gameObject_static))
	{
		//TODO: Open Popup: Warning parent change, asking children
		gameObject->SetStatic(gameObject_static, false);
	}

	ImGui::Unindent();

	ImGui::Separator();
	ImGui::Separator();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;

	C_Transform* transform = gameObject->GetComponent<C_Transform>();
	DrawTransform(gameObject, transform);

	C_Mesh* mesh = gameObject->GetComponent<C_Mesh>();
	DrawMesh(gameObject, mesh);

	DrawMaterials(gameObject);

	C_Camera* camera = gameObject->GetComponent<C_Camera>();
	DrawCamera(gameObject, camera);

	C_Animator* animator = gameObject->GetComponent<C_Animator>();
	DrawAnimator(gameObject, animator);

	C_Billboard* billboard = gameObject->GetComponent<C_Billboard>();
	DrawBillboard(gameObject, billboard);

	C_ParticleSystem* particleSystem = gameObject->GetComponent<C_ParticleSystem>();
	DrawParticleSystem(gameObject, particleSystem);

	ImGui::Separator();
	ImGui::Separator();

	int buttonPos_x = ImGui::GetWindowWidth() / 2 - 100;
	ImGui::SetCursorPosX(buttonPos_x);
	if (ImGui::Button("Add Component", ImVec2(200, 0)))
	{
		ImGui::OpenPopup("Add Component Popup");
	}

	if (ImGui::BeginPopup("Add Component Popup"))
	{
		if (ImGui::MenuItem("Mesh"))
		{
			gameObject->CreateComponent(Component::Type::Mesh);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Material"))
		{
			gameObject->CreateComponent(Component::Type::Material);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Camera"))
		{
			gameObject->CreateComponent(Component::Type::Camera);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Animation"))
		{
			gameObject->CreateComponent(Component::Type::Animator);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Billboard"))
		{
			gameObject->CreateComponent(Component::Type::Billboard);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Particle System"))
		{
			gameObject->CreateComponent(Component::Type::ParticleSystem);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

}

void W_Inspector::DrawTransform(GameObject* gameObject, C_Transform* transform)
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		if (ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[1])
		{
			ImGui::OpenPopup("reset");
		}

		if (ImGui::BeginPopup("reset"))
		{
			if (ImGui::Button("Reset"))
			{
				transform->Reset();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		float3 pos = transform->GetPosition();
		float3 scale = transform->GetScale();
		float3 rotation = transform->GetEulerRotation();

		if (DrawDetails_Float3("Position", pos)) transform->SetPosition(pos);
		if (DrawDetails_Float3("Rotation", rotation)) transform->SetEulerRotation(rotation);
		if (DrawDetails_Float3("Scale", scale)) transform->SetScale(scale);

		if (showDebugInfo)
		{
			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text("Local Matrix");
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; j++)
				{
					ImGui::Text("%f", transform->GetTransform()[j][i]);
					if (j < 3) ImGui::SameLine();
				}
			}
			ImGui::PopStyleColor();
			ImGui::Separator();

			ImGui::Text("Global Matrix");
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; j++)
				{
					ImGui::Text("%f", transform->GetGlobalTransform()[j][i]);
					if (j < 3) ImGui::SameLine();
				}
			}
			ImGui::PopStyleColor();
			ImGui::Separator();
		}
		ImGui::Unindent();
	}
}

void W_Inspector::DrawMesh(GameObject* gameObject, C_Mesh* mesh)
{
	if (mesh == nullptr) return;
	R_Mesh* rMesh = mesh->rMeshHandle.Get();

	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		if (uint64 newID = DrawDetails_Resource("Mesh", mesh->rMeshHandle.Get()))
		{
			mesh->SetResource(newID);
		}
		ImGui::Unindent();
	}
}

void W_Inspector::DrawMaterials(GameObject* gameObject)
{
	std::vector<C_Material*> materials;
	gameObject->GetComponents(materials);

	if (materials.size() == 0) return;
	if (ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		if (materials.size() > 0)
		{
			for (uint i = 0; i < materials.size(); i++)
			{
				DrawMaterial(gameObject, materials[i], i);
			}
		}

		ImGui::Unindent();
	}
}

void W_Inspector::DrawMaterial(GameObject* gameObject, C_Material* material, uint index)
{
	if (material == nullptr) return;

	std::string name = std::string("Element ") + std::to_string(index);
	if (uint64 newID = DrawDetails_Resource(name.c_str(), material->rMaterialHandle.Get()))
	{
		material->SetResource(newID);
	}
}

void W_Inspector::DrawCamera(GameObject* gameObject, C_Camera* camera)
{
	if (camera == nullptr) return;

	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		if (ImGui::Button("Match camera"))
		{
			Engine->camera->Match(camera);
		}

		if (DrawDetails_CheckBox("Camera Culling", camera->culling))
		{
			camera->culling ? Engine->renderer3D->SetCullingCamera(camera) : Engine->renderer3D->SetCullingCamera(nullptr);
		}

		float camera_fov = camera->GetFOV();
		if (DrawDetails_Float("Field of View", camera_fov))
		{
			camera->SetFOV(camera_fov);
		}

		float camera_near_plane = camera->GetNearPlane();
		if (DrawDetails_Float("Near plane", camera_near_plane))
		{
			camera->SetNearPlane(camera_near_plane);
		}

		float camera_far_plane = camera->GetFarPlane();
		if (DrawDetails_Float("Far plane", camera_far_plane))
		{
			camera->SetFarPlane(camera_far_plane);
		}

		ImGui::Unindent();
	}
}

void W_Inspector::DrawAnimator(GameObject* gameObject, C_Animator* animator)
{
	if (animator == nullptr) return;

	//TODO: Animations should be displayed in a new window now, which would be a timeline

	if (ImGui::CollapsingHeader("Animator", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		ImGui::Text("Animations");
		ImGui::Separator();

		R_AnimatorController* rAnimator = animator->rAnimatorHandle.Get();
		for (uint i = 0; i < rAnimator->animations.size(); ++i)
		{
			//TODO: Animations should all be loaded with animation controller

			//R_Animation* animation = (R_Animation*)Engine->moduleResources->GetResource(rAnimator->animations[i]);
			//ImGui::Text(animation ? animation->name.c_str() : "Empty Animation");
		}

		if (ImGui::Button("Add Animation"))
		{
			rAnimator->AddAnimation();
		}

		ImGui::Unindent();
	}
}

void W_Inspector::DrawBillboard(GameObject* gameObject, C_Billboard* billboard)
{
	if (billboard == nullptr) return;

	if (ImGui::CollapsingHeader("Billboard", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		int currentAlignmentOption = (int)billboard->alignment;
		if (ImGui::Combo("Alignment", &currentAlignmentOption, billboardAlignmentOptions.data(), (int)C_Billboard::Alignment::Unknown))
		{
			billboard->alignment = (C_Billboard::Alignment)currentAlignmentOption;
		}
		if (billboard->alignment == C_Billboard::Alignment::Axis_Aligned)
		{
			int currentLockOption = (int)billboard->lockAxis;
			if (ImGui::Combo("Axis Lock", &currentLockOption, billboardLockOptions.data(), (int)C_Billboard::Axis::Unknown))
			{
				billboard->lockAxis = (C_Billboard::Axis)currentLockOption;
			}
		}

		ImGui::Unindent();
	}
}

void W_Inspector::DrawParticleSystem(GameObject* gameObject, C_ParticleSystem* particleSystem)
{
	if (particleSystem == nullptr) return;

	if (ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		R_ParticleSystem* resource = particleSystem->rParticleSystemHandle.Get();
		std::string menuName = resource ? resource->GetName() : "-- Select Particle System --";

		if (ImGui::BeginMenu(menuName.c_str()))
		{
			if (ImGui::MenuItem("Create New"))
			{
				W_Explorer* w_explorer = (W_Explorer*)editor->GetWindowFrame(WF_SceneEditor::GetName())->GetWindow(W_Explorer::GetName());
				const char* targetDir = Engine->moduleEditor->GetCurrentExplorerDirectory();
				if (uint64 resourceID = Engine->moduleResources->CreateNewCopyResource("Engine/Assets/Defaults/New Particle System.particles", targetDir))
				{
					particleSystem->SetResource(resourceID);
				}
			}
			
			std::vector<const ResourceBase*> resourceBases;
			if (Engine->moduleResources->GetAllMetaFromType(ResourceType::PARTICLESYSTEM, resourceBases))
			{
				ImGui::Separator();
				for (uint i = 0; i < resourceBases.size(); ++i)
				{
					if (ImGui::MenuItem(resourceBases[i]->name.c_str()))
					{
						particleSystem->SetResource(resourceBases[i]->ID);
					}
				}
			}
			ImGui::EndMenu();
		}
		ImGui::Unindent();

		if (showDebugInfo)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
			ImGui::Separator();
			ImGui::Text("Emitters:");
			ImGui::Indent();

			for (uint i = 0; i < particleSystem->emitters.size(); ++i)
			{
				ImGui::Text(particleSystem->emitters[i].emitterReference->name.c_str());
				ImGui::Text("Particles");
				ImGui::Indent();

				EmitterInstance* instance = &particleSystem->emitters[i];
				for (uint i = 0; i < instance->activeParticles; ++i)
				{
					unsigned int particleIndex = instance->particleIndices[i];
					Particle* particle = &instance->particles[particleIndex];

					ImGui::Text("Particle %i - %f", particleIndex, particle->relativeLifetime);
				}

				ImGui::Unindent();
				ImGui::Separator();
			}

			ImGui::PopStyleColor();
			ImGui::Unindent();
		}
	}
}