#include "W_Resources.h"

#include "ImGui/imgui.h"

#include "Application.h"
#include "M_Resources.h"

//Resources
#include "R_Mesh.h"
#include "R_Material.h"
#include "R_Texture.h"
#include "R_Prefab.h"

W_Resources::W_Resources(M_Editor* editor) : DWindow(editor, "Resources")
{


}

void W_Resources::Draw()
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
}
void W_Resources::OnResize()
{

}

void W_Resources::DisplayResourceInfo(Resource* resource)
{
	ImGui::BeginTooltip();
	ImGui::Text("UID: %llu", resource->GetID());
	ImGui::Text("Source file: %s", resource->GetOriginalFile());
	ImGui::Text("Instances: %i", resource->instances);
	ImGui::EndTooltip();
}