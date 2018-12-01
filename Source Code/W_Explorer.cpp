#include "W_Explorer.h"

#include "ImGui/imgui.h"
#include "Glew/include/glew.h"

#include "Application.h"
#include "M_Resources.h"
#include "M_FileSystem.h"

#include "R_Texture.h"

W_Explorer::W_Explorer(M_Editor* editor) : DWindow(editor, "Explorer")
{
	updateTimer.Start();
	UpdateTree();
	currentNode = assets;
}

void W_Explorer::Draw()
{
	if (updateTimer.ReadSec() > updateTime)
	{
		UpdateTree();
		updateTimer.Start();
	}

	DrawFolderNode(assets);
}

void W_Explorer::OnResize()
{

}

void W_Explorer::DrawFolderNode(const PathNode& node)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (node.IsLastFolder() == true)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;
		nodeFlags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	if (currentNode == node)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	//If node folder has something inside
	if (node.file == false)
	{
		bool open = ImGui::TreeNodeEx(node.localPath.c_str(), nodeFlags, node.localPath.c_str());
		if (ImGui::IsItemClicked())
			currentNode = node;
		if (open && !node.IsLastFolder())
		{
			for (uint i = 0; i < node.children.size(); i++)
			{
				DrawFolderNode(node.children[i]);
			}
			ImGui::TreePop();
		}
	}

}

void W_Explorer::DrawNodeImage(const PathNode& node)
{
	if (node.file == false)
		ImGui::Image((ImTextureID)folderBuffer, ImVec2(imageSize, imageSize));
	else
	{
		std::string metaFile = node.path + (".meta");
		uint64 id = App->moduleResources->GetIDFromMeta(metaFile.c_str());
		Resource* resource = App->moduleResources->GetResource(id);

		if (resource && resource->GetType() == Resource::TEXTURE)
		{
			R_Texture* tex = (R_Texture*)resource;
			ImGui::Image((ImTextureID)tex->buffer, ImVec2(imageSize, imageSize));
		}
		else if (resource && resource->GetType() == Resource::PREFAB)
		{
			//R_Prefab* prefab = (R_Prefab*)resource;
			//R_Texture* tex = (R_Texture*)App->moduleResources->GetResource(prefab->miniTextureID);
			//if (tex)
			//	ImGui::Image((ImTextureID)tex->buffer, ImVec2(imageSize, imageSize));

			//Not saving prefab screenshot by now
			ImGui::Image((ImTextureID)fileBuffer, ImVec2(imageSize, imageSize));
		}
		else
		{
			ImGui::Image((ImTextureID)fileBuffer, ImVec2(imageSize, imageSize));
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0); //Soo... this needs to be done in order to reset the texture buffer

}

void W_Explorer::UpdateTree()
{
	std::vector<std::string> ignore_ext;
	ignore_ext.push_back("meta");
	assets = App->fileSystem->GetAllFiles("Assets", nullptr, &ignore_ext);
}

std::string W_Explorer::GetTextAdjusted(const char* text)
{
	std::string newText = text;
	uint textLenght = newText.size();

	const char* text_end = text + textLenght;
	uint textSizeX = ImGui::CalcTextSize(text, text_end, false, 0).x;

	if (textSizeX > imageSize)
	{
		uint charSize = textSizeX / textLenght;
		uint newLenght = (imageSize / charSize) - 1;
		newText = newText.substr(0, newLenght);
		newText.append("...");
	}
	return newText;
}