#include "P_Resources.h"
#include "Application.h"
#include "M_Resources.h"

//Resources
#include "R_Mesh.h"
#include "R_Material.h"
#include "R_Texture.h"
#include "R_Prefab.h"

#include "M_FileSystem.h"

P_Resources::P_Resources()
{
}

P_Resources::~P_Resources()
{
}

void P_Resources::Draw(ImGuiWindowFlags flags)
{
	if (active)
	{
		ImGui::SetNextWindowPos(position);
		ImGui::SetNextWindowSize(size);

		if (!ImGui::Begin("Resources (read-only)", &active, ImVec2(size), 1.0f, flags))
		{
			ImGui::End();
			return;
		}
		else
		{
			ImGui::Text("Resources loaded in memory");
			
			if (ImGui::CollapsingHeader("Scenes"))
			{
				for (std::map<uint64, Resource*>::iterator it = App->moduleResources->scenes.begin(); it != App->moduleResources->scenes.end(); it++)
				{
					ImGui::Text("-- %s", it->second->GetName());
					if (ImGui::IsItemHovered())
					{
						DisplayResourceInfo(it->second);
					}
				}
			}

			if (ImGui::CollapsingHeader("Meshes"))
			{
				for (std::map<uint64, Resource*>::iterator it = App->moduleResources->meshes.begin(); it != App->moduleResources->meshes.end(); it++)
				{
					ImGui::Text("-- %s", it->second->GetName());
					if (ImGui::IsItemHovered())
					{
						DisplayResourceInfo(it->second);
					}
				}
			}

			if (ImGui::CollapsingHeader("Materials"))
			{
				for (std::map<uint64, Resource*>::iterator it = App->moduleResources->materials.begin(); it != App->moduleResources->materials.end(); it++)
				{
					ImGui::Text("-- %s", it->second->GetName());
					if (ImGui::IsItemHovered())
					{
						DisplayResourceInfo(it->second);
					}
				}
			}

			if (ImGui::CollapsingHeader("Textures"))
			{
				for (std::map<uint64, Resource*>::iterator it = App->moduleResources->textures.begin(); it != App->moduleResources->textures.end(); it++)
				{
					ImGui::Text("-- %s", it->second->GetName());
					if (ImGui::IsItemHovered())
					{
						DisplayResourceInfo(it->second);
					}
				}
			}

			if (ImGui::CollapsingHeader("Animations"))
			{
				for (std::map<uint64, Resource*>::iterator it = App->moduleResources->animations.begin(); it != App->moduleResources->animations.end(); it++)
				{
					ImGui::Text("-- %s", it->second->GetName());
					if (ImGui::IsItemHovered())
					{
						DisplayResourceInfo(it->second);
					}
				}
			}
			ImGui::End();
		}
	}
}

void P_Resources::DisplayResourceInfo(Resource* resource)
{
	ImGui::BeginTooltip();
	ImGui::Text("UID: %llu", resource->GetID());
	ImGui::Text("Source file: %s", resource->GetOriginalFile());
	ImGui::Text("Instances: %i", resource->instances);
	ImGui::EndTooltip();
}

void P_Resources::UpdatePosition(int screen_width, int screen_height)
{
	position.x = 0;
	position.y = screen_height * (0.60);

	size.x = screen_width * (0.20);
	size.y = screen_height * (1.00) - position.y;
}