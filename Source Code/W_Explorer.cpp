#include "W_Explorer.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include "Glew/include/glew.h"

#include "Application.h"
#include "M_Resources.h"
#include "M_FileSystem.h"
#include "M_Editor.h"

#include "R_Texture.h"

#include "Dock.h"


W_Explorer::W_Explorer(M_Editor* editor) : DWindow(editor, "Explorer")
{
	updateTimer.Start();
	UpdateTree();
	currentNode = assets;
	allowScrollbar = false;
}

void W_Explorer::Draw()
{
	if (updateTimer.ReadSec() > updateTime)
	{
		UpdateTree();
		updateTimer.Start();
	}

	ImGui::PushID(this);

	ImGui::BeginChild("ExplorerTree", ImVec2(windowSize.x * 0.2f, windowSize.y));
	DrawFolderNode(assets);
	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginChild("ExplorerFolder", ImVec2(windowSize.x * 0.8f, windowSize.y), false, ImGuiWindowFlags_NoScrollbar);
	DrawSelectedFolderContent();
	ImGui::EndChild();

	ImGui::PopID();
}

void W_Explorer::OnResize()
{
	columnsNumber = ((parent->size.x * 0.8f ) * 10) / 1152;
	windowSize = parent->size - Vec2(0.0f, 25.0f);
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

	uint64 resourceID = 0;
	std::string dnd_event("NONE");
	uint textureID = GetTextureFromNode(node, &resourceID, &dnd_event);

	ImGui::Image((ImTextureID)textureID, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		if (dnd_event != "NONE")
		{
			ImGui::SetDragDropPayload(dnd_event.c_str(), &resourceID, sizeof(uint64));
			ImGui::Text(node.localPath.c_str());
			ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(0.0f, 20.0f));
			ImGui::Image((ImTextureID)textureID, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::EndDragDropSource();
	}

	glBindTexture(GL_TEXTURE_2D, 0); //Soo... this needs to be done in order to reset the texture buffer
}

void W_Explorer::DrawSelectedFolderContent()
{
	nextCurrent.path = "";

	ImGui::BeginChild("ExplorerFolder", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_NoScrollbar);

	ImGui::Text(currentNode.path.c_str());
	ImGui::Separator();

	ImGui::BeginChild("ImagesChild");
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(50, 30));

	ImVec2 vec = ImGui::GetCursorPos();

	uint line = 0;

	for (uint i = 0; i < currentNode.children.size(); i++)
	{
		ImGui::PushID(i);

		ImGui::SetCursorPosX(vec.x + (i - (line * columnsNumber)) * (imageSize + imageSpacingX) + imageSpacingX);
		ImGui::SetCursorPosY(vec.y + line * (imageSize + imageSpacingY) + topMarginOffset);

		DrawNodeImage(currentNode.children[i]);

		if (explorerSelected == currentNode.children[i])
		{
			ImGui::SetCursorPosX(vec.x + (i - line * columnsNumber) * (imageSize + imageSpacingX) + imageSpacingX - 10.0f);
			ImGui::SetCursorPosY(vec.y + line * (imageSize + imageSpacingY));

			float textSize = ImGui::GetTextLineHeight();
			ImGui::Image((ImTextureID)selectedBuffer, ImVec2(imageSize + 20.0f, imageSize + textSize + 24.0f), ImVec2(0, 1), ImVec2(1, 0));
		}

		if (ImGui::IsItemClicked())
		{
			explorerSelected = currentNode.children[i];

			//TODO: quick workaround to select resources
			App->moduleEditor->selectedResources.clear();
			std::string metaFile = explorerSelected.path + (".meta");
			uint64 resourceID = App->moduleResources->GetIDFromMeta(metaFile.c_str());
			if (Resource* resource = App->moduleResources->GetResource(resourceID))
				App->moduleEditor->selectedResources.push_back(resource);
		}
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) 
		{
			HandleNodeDoubleClick(currentNode.children[i]);
		}

		ImGui::SetCursorPosX(vec.x + (i - line * columnsNumber) * (imageSize + imageSpacingX) + imageSpacingX);
		ImGui::SetCursorPosY(vec.y + line * (imageSize + imageSpacingY) + imageSize + textOffset + topMarginOffset);

		std::string textAdjusted = GetTextAdjusted(currentNode.children[i].localPath.c_str());
		ImGui::Text(textAdjusted.c_str());

		if ((i + 1) % columnsNumber == 0)
			line++;

		ImGui::PopID();
	}
	ImGui::PopStyleVar();
	ImGui::EndChild();

	if (nextCurrent.path != "")
		currentNode = nextCurrent;

	ImGui::EndChild();
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
	uint dotsSizeX = ImGui::CalcTextSize("...", nullptr, false, 0).x;

	if (textSizeX > imageSize + 15.0f)
	{
		uint charSize = textSizeX / textLenght;
		uint newLenght = ((imageSize + 15.0f - dotsSizeX) / charSize) - 1;
		newText = newText.substr(0, newLenght);
		newText.append("...");
	}
	return newText;
}

void W_Explorer::HandleNodeDoubleClick(const PathNode& node)
{
	if (node.file == false)
		nextCurrent = node;
	else
	{
		std::string metaFile = node.path + (".meta");
		uint64 id = App->moduleResources->GetIDFromMeta(metaFile.c_str());
		Resource* resource = App->moduleResources->GetResource(id);

		if (resource && resource->GetType() == Resource::PREFAB)
		{
			App->moduleResources->LoadPrefab(id);
		}
	}
}

uint W_Explorer::GetTextureFromNode(const PathNode& node, uint64* resource_id, std::string* dnd_event)
{
	uint64 resourceID = 0;

	if (node.file == false)
	{
		if (dnd_event) dnd_event->assign("DND_FOLDER");
		return folderBuffer;
	}
	else
	{
		//TODO: this should be done in a more fancy way than opening the meta 2 times.
		std::string metaFile = node.path + (".meta");
		Resource::Type type = App->moduleResources->GetTypeFromMeta(metaFile.c_str());

		if (resource_id) *resource_id = App->moduleResources->GetIDFromMeta(metaFile.c_str());

		switch (type)
		{
		case(Resource::TEXTURE):
			{
				if (dnd_event) dnd_event->assign("DND_TEXTURE");
				//return ((R_Texture*)resource)->buffer;
				return fileBuffer;
			}
		case(Resource::PREFAB):
			{
					if (dnd_event) dnd_event->assign("DND_PREFAB");
					//R_Prefab* prefab = (R_Prefab*)resource;
					//R_Texture* tex = (R_Texture*)App->moduleResources->GetResource(prefab->miniTextureID);
					//if (tex)
					//	ImGui::Image((ImTextureID)tex->buffer, ImVec2(imageSize, imageSize));

					//Not saving prefab screenshot by now
					return fileBuffer;
			}
		default:
			return fileBuffer;
		}
	}
}