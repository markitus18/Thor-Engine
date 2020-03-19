#include "W_Explorer.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include "Glew/include/glew.h"

#include "Application.h"
#include "M_Resources.h"
#include "M_FileSystem.h"
#include "M_Editor.h"

#include "Resource.h"
#include "R_Texture.h"
#include "R_Prefab.h"
#include "R_Folder.h"

#include "Dock.h"


W_Explorer::W_Explorer(M_Editor* editor) : DWindow(editor, "Explorer")
{
	updateTimer.Start();
	UpdateTree();

	allowScrollbar = false;

	Resource* resource = App->moduleResources->GetResource(App->moduleResources->GetResourceInfo("Assets").id);
	currentFolder = assetsFolder = (R_Folder*)resource;

	resource = App->moduleResources->GetResource(App->moduleResources->GetResourceInfo("Engine/Assets").id);
	engineAssetsFolder = (R_Folder*)resource;
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

void W_Explorer::DrawFolderNode(PathNode& node)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (node.IsLastFolder() == true)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;
		nodeFlags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	if (currentFolder && node.path == currentFolder->GetOriginalFile())
	{
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	//If node folder has something inside
	if (node.isFile == false)
	{
		bool open = ImGui::TreeNodeEx(node.localPath.c_str(), nodeFlags, node.localPath.c_str());
		if (ImGui::IsItemClicked())
		{
			Resource* resource = App->moduleResources->GetResource(App->moduleResources->GetResourceInfo(node.path.c_str()).id);
			currentFolder = (R_Folder*)resource;
		}
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

void W_Explorer::DrawResourceImage(const Resource* resource)
{
	uint64 resourceID = resource->GetID();
	std::string dnd_event("NONE");
	uint textureID = GetTextureFromResource(resource, &dnd_event);

	ImGui::Image((ImTextureID)textureID, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		if (dnd_event != "NONE")
		{
			ImGui::SetDragDropPayload(dnd_event.c_str(), &resourceID, sizeof(uint64));
			ImGui::Text(resource->name.c_str());
			ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(0.0f, 20.0f));
			ImGui::Image((ImTextureID)textureID, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::EndDragDropSource();
	}

	glBindTexture(GL_TEXTURE_2D, 0); //Soo... this needs to be done in order to reset the texture buffer
}

//TODO: function needs some cleaning. Pathnodes force GetResource constantly which means can't free resource memory properly
void W_Explorer::DrawSelectedFolderContent()
{
	nextCurrentFolder = nullptr;

	ImGui::BeginChild("ExplorerFolder", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_NoScrollbar);

	ImGui::Text(currentFolder->GetOriginalFile());
	ImGui::Separator();

	ImGui::BeginChild("ImagesChild");
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(50, 30));

	ImVec2 windowCursorPosition = ImGui::GetCursorPos();
	uint itemIndex = 0;

	for (uint i = 0; i < currentFolder->containingResources.size(); ++i)
	{
		Resource* res = App->moduleResources->GetResource(currentFolder->containingResources[i]);
		DrawResourceItem(res, itemIndex, windowCursorPosition);
		itemIndex++;
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();

	if (nextCurrentFolder != nullptr)
		currentFolder = nextCurrentFolder;

	ImGui::EndChild();
}

void W_Explorer::DrawResourceItem(Resource* resource, uint& itemIndex, ImVec2 windowCursorPos)
{
	uint row = itemIndex / columnsNumber;
	uint column = itemIndex - (row * columnsNumber);

	ImVec2 drawPos(windowCursorPos.x + column * (imageSize + imageSpacingX) + imageSpacingX,
		windowCursorPos.y + row * (imageSize + imageSpacingY) + topMarginOffset);

	ImGui::PushID(row * column);
	ImGui::SetCursorPos(drawPos);

	DrawResourceImage(resource);

	//Draw selection image
	if (selectedResource == resource)
	{
		ImGui::SetCursorPos(drawPos - ImVec2(topMarginOffset, topMarginOffset));
		float textSize = ImGui::GetTextLineHeight();
		uint texBuffer = ((R_Texture*)App->moduleResources->GetResource(selectedResourceImage))->buffer;
		ImGui::Image((ImTextureID)texBuffer, ImVec2(imageSize + 20.0f, imageSize + textSize + 24.0f), ImVec2(0, 1), ImVec2(1, 0));
	}

	//If clicked, select resource to display in inspector
	if (ImGui::IsItemClicked())
	{
		selectedResource = resource;

		//TODO: quick workaround to select resources
		App->moduleEditor->selectedResources.clear();
		App->moduleEditor->selectedResources.push_back(resource);
	}
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
	{
		HandleResourceDoubleClick(resource);
	}

	ImVec2 textPos = drawPos + ImVec2(0.0f, imageSize + textOffset);
	ImGui::SetCursorPos(textPos);
	ImGui::Text(GetTextAdjusted(resource->GetName()).c_str());

	//Drawing node Button (if it's a prefab)
	if (resource->GetType() == Resource::PREFAB)
	{
		R_Prefab* prefabNode = (R_Prefab*)resource;

		ImGui::SetCursorPos(drawPos + ImVec2(imageSize / 2 + nodeButtonOffset, imageSize / 2 - ImGui::GetFrameHeight() / 2));
		ImGui::ArrowButton("ArrowButton?", prefabNode == openPrefab ? ImGuiDir_Left : ImGuiDir_Right);
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
		{
			openPrefab = openPrefab == prefabNode ? nullptr : prefabNode;
		}

		if (prefabNode == openPrefab)
		{
			//Draw the resources contained in the prefab
			for (uint i = 0; i < prefabNode->containingResources.size(); ++i)
			{
				itemIndex++;
				Resource* containedResource = App->moduleResources->GetResource(prefabNode->containingResources[i]);
				DrawResourceItem(containedResource, itemIndex, windowCursorPos);
			}
		}
	}

	ImGui::PopID();
}

void W_Explorer::UpdateTree()
{
	std::vector<std::string> ignore_ext;
	ignore_ext.push_back("meta");
	assets = App->fileSystem->GetAllFiles("Assets", nullptr, &ignore_ext);
}

std::string W_Explorer::GetTextAdjusted(const char* text) const
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

void W_Explorer::HandleResourceDoubleClick(const Resource* resource)
{
	if (resource->GetType() == Resource::FOLDER)
		nextCurrentFolder = (R_Folder*)resource;
	else
	{
		if (resource->GetType() == Resource::PREFAB)
		{
			App->moduleResources->LoadPrefab(resource->GetID());
		}
	}
}

uint W_Explorer::GetTextureFromResource(const Resource* resource, std::string* dnd_event)
{
	uint64 textureBuffer = 0;
	if (resource->GetType() == Resource::TEXTURE)
	{
		textureBuffer = ((R_Texture*)resource)->buffer;
	}
	else
	{
		R_Texture* texture = (R_Texture*)App->moduleResources->GetResource(resourceIcons[resource->GetType()]);
		textureBuffer = texture->buffer;
	}
	dnd_event->assign("DND_RESOURCE_").append(std::to_string((int)resource->GetType()));

	return textureBuffer;
}