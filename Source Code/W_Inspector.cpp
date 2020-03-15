#include "W_Inspector.h"

#include "ImGui/imgui.h"

#include "M_Editor.h"
#include "Application.h"
#include "M_Input.h"
#include "M_Scene.h"
#include "M_Resources.h"
#include "M_Camera3D.h"
#include "M_Renderer3D.h"
#include "W_Explorer.h"

#include "TreeNode.h"
#include "GameObject.h"

#include "C_Mesh.h"
#include "C_Transform.h" //TODO: why wasnt it necessary?
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

W_Inspector::W_Inspector(M_Editor* editor) : DWindow(editor, "Inspector")
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
	if (editor->selectedGameObjects.size() > 0)
	{
		DrawGameObject((GameObject*)editor->selectedGameObjects[0]);
	}

	
}

void W_Inspector::OnResize()
{

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
		App->scene->SetStaticGameObject(gameObject, gameObject_static, true);
	}

	ImGui::Unindent();

	ImGui::Separator();
	ImGui::Separator();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;

	C_Transform* transform = gameObject->GetComponent<C_Transform>();
	DrawTransform(gameObject, transform);

	C_Mesh* mesh = gameObject->GetComponent<C_Mesh>();
	DrawMesh(gameObject, mesh);

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

	if (ImGui::Button("Add Component"))
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

		if (ImGui::DragFloat3("Position", (float*)&pos, 0.15f))
		{
			App->input->InfiniteHorizontal();
			transform->SetPosition(pos);
		}

		if (ImGui::DragFloat3("Rotation", (float*)&rotation, 0.5f))
		{
			App->input->InfiniteHorizontal();
			transform->SetEulerRotation(rotation);
		}

		if (ImGui::DragFloat3("Scale", (float*)&scale, 0.15f))
		{
			App->input->InfiniteHorizontal();
			transform->SetScale(scale);
		}

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
	std::vector<C_Material*> materials;
	gameObject->GetComponents(materials);
	if (mesh == nullptr) return;

	R_Mesh* rMesh = (R_Mesh*)mesh->GetResource();

	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		ImGui::Text("Mesh"); ImGui::SameLine();
		
		if (ImGui::Button(rMesh ? rMesh->name.c_str() : "", ImVec2(ImGui::GetWindowSize().x - ImGui::GetCursorPosX(), 0)));
		{
			//TODO: Open window with meshes list
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_MESH"))
			{
				if (payload->DataSize == sizeof(uint64))
				{
					uint64 resourceID = *(const uint64*)payload->Data;
					Resource* resource = App->moduleResources->GetResource(resourceID);

					if (resource->GetType() == Resource::Type::MESH)
					{
						mesh->SetResource(resource);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Separator();
		ImGui::Text("Materials");
		ImGui::Separator();
		ImGui::Text("Size: %i", materials.size());
		for (uint i = 0; i < materials.size(); i++)
		{
			R_Material* rMat = (R_Material*)materials[i]->GetResource();
			ImGui::Text("Element %i: %s", i, rMat->GetName());
		}

		ImGui::Unindent();
	}

	if (materials.size() > 0)
	{
		for (uint i = 0; i < materials.size(); i++)
		{
			R_Material* rMat = (R_Material*)materials[i]->GetResource();
			DrawMaterial(gameObject, rMat);
		}
	}
}

void W_Inspector::DrawMaterial(GameObject* gameObject, R_Material* material)
{
	if (material == nullptr) return;

	if (ImGui::CollapsingHeader(material->GetName(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		if (material->textureID != 0)
		{
			R_Texture* rTex = (R_Texture*)App->moduleResources->GetResource(material->textureID);
			if (rTex)
			{
				ImGui::Text(rTex->GetName());
				ImGui::Image((ImTextureID)rTex->buffer, ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TEXTURE"))
					{
						if (payload->DataSize == sizeof(uint64))
						{
							uint64 resourceID = *(const uint64*)payload->Data;
							Resource* resource = App->moduleResources->GetResource(resourceID);

							if (resource->GetType() == Resource::Type::TEXTURE)
							{
								material->textureID = resourceID;
							}
						}
					}
					ImGui::EndDragDropTarget();
				}
			}
		}

		float color[3] = { material->color.r, material->color.g, material->color.b };
		if (ImGui::ColorEdit3("Color", color))
		{
			material->color.Set(color[0], color[1], color[2]);
			material->needs_save = true;
		}

		ImGui::Unindent();
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
			App->camera->Match(camera);
		}

		if (ImGui::Checkbox("Set View Camera", &camera->active_camera))
		{
			camera->active_camera ? App->renderer3D->SetActiveCamera(camera) : App->renderer3D->SetActiveCamera(nullptr);
		}

		if (ImGui::Checkbox("Camera Culling", &camera->culling))
		{
			camera->culling ? App->renderer3D->SetCullingCamera(camera) : App->renderer3D->SetCullingCamera(nullptr);
		}

		//TODO: move all frustum functions to C_Camera and call from there
		float camera_fov = camera->GetFOV();
		if (ImGui::DragFloat("Field of View", (float*)&camera_fov))
		{
			camera->SetFOV(camera_fov);
		}

		float camera_near_plane = camera->GetNearPlane();
		if (ImGui::DragFloat("Near plane", &camera_near_plane))
		{
			camera->SetNearPlane(camera_near_plane);
		}

		float camera_far_plane = camera->GetFarPlane();
		if (ImGui::DragFloat("Far plane", &camera_far_plane))
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

		R_AnimatorController* rAnimator = (R_AnimatorController*)animator->GetResource();
		for (uint i = 0; i < rAnimator->animations.size(); ++i)
		{
			R_Animation* animation = (R_Animation*)App->moduleResources->GetResource(rAnimator->animations[i]);
			ImGui::Text(animation ? animation->name.c_str() : "Empty Animation");
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

		R_ParticleSystem* resource = (R_ParticleSystem*)particleSystem->GetResource();
		std::string menuName = resource ? resource->GetName() : "-- Select Particle System --";

		if (ImGui::BeginMenu(menuName.c_str()))
		{
			if (ImGui::MenuItem("Create New"))
			{
				const char* dir = editor->w_explorer->GetCurrentFolder()->GetOriginalFile();
				if (uint64 resourceID = App->moduleResources->CreateNewCopyResource(dir, "Engine/Assets/Defaults/New Particle System.particles", Resource::Type::PARTICLESYSTEM))
				{
					particleSystem->SetResource(resourceID);
				}
			}
			
			std::vector<const ResourceMeta*> resourceMetas;
			if (App->moduleResources->GetAllMetaFromType(Resource::Type::PARTICLESYSTEM, resourceMetas))
			{
				ImGui::Separator();
				for (uint i = 0; i < resourceMetas.size(); ++i)
				{
					if (ImGui::MenuItem(resourceMetas[i]->resource_name.c_str()))
					{
						particleSystem->SetResource(resourceMetas[i]->id);
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