#include "W_Explorer.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include "Glew/include/glew.h"

#include "Engine.h"
#include "M_Resources.h"
#include "M_FileSystem.h"
#include "M_Editor.h"

#include "Resource.h"
#include "R_Texture.h"
#include "R_Prefab.h"
#include "R_Folder.h"

W_Explorer::W_Explorer(M_Editor* editor) : Window(editor, "Explorer")
{
	updateTimer.Start();
	UpdateTree();

	allowScrollbar = false;

	Resource* resource = Engine->moduleResources->GetResource(Engine->moduleResources->GetResourceInfo("Assets").ID);
	currentFolder = assetsFolder = (R_Folder*)resource;

	resource = Engine->moduleResources->GetResource(Engine->moduleResources->GetResourceInfo("Engine/Assets").ID);
	engineAssetsFolder = (R_Folder*)resource;

	resourceIcons[Resource::Type::FOLDER] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/FolderIcon.png").ID;
	resourceIcons[Resource::Type::MESH] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/FileIcon.png").ID;
	resourceIcons[Resource::Type::TEXTURE] = 0;
	resourceIcons[Resource::Type::MATERIAL] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/MaterialIcon.png").ID;
	resourceIcons[Resource::Type::ANIMATION] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/AnimationIcon.png").ID;
	resourceIcons[Resource::Type::ANIMATOR_CONTROLLER] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/AnimatorIcon.png").ID;
	resourceIcons[Resource::Type::PREFAB] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/SceneIcon.png").ID;
	resourceIcons[Resource::Type::PARTICLESYSTEM] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/ParticlesIcon.png").ID;
	resourceIcons[Resource::Type::SHADER] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/ShaderIcon.png").ID;
	resourceIcons[Resource::Type::SCENE] = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/ThorIcon.png").ID;
	selectedResourceImage = Engine->moduleResources->GetResourceInfo("Engine/Assets/Icons/SelectedIcon.png").ID;
}

void W_Explorer::Draw()
{
	ImGui::SetNextWindowClass(editor->normalWindowClass);
	if (!ImGui::Begin("Explorer", &active)) { ImGui::End(); return; }

	ImVec2 winSize = ImGui::GetWindowSize();
	if (winSize.x != windowSize.x || winSize.y != windowSize.y)
		OnResize(Vec2(winSize.x, winSize.y));

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

	ImGui::End();
}

void W_Explorer::OnResize(Vec2 newSize)
{
	windowSize = newSize;
	columnsNumber = ((newSize.x * 0.8f ) * 10) / 1152;
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
			Resource* resource = Engine->moduleResources->GetResource(Engine->moduleResources->GetResourceInfo(node.path.c_str()).ID);
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

	for (uint i = 0; i < currentFolder->containedResources.size(); ++i)
	{
		Resource* res = Engine->moduleResources->GetResource(currentFolder->containedResources[i]);
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
	if (columnsNumber == 0) return; //No space to draw any resource, exit function

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
		uint texBuffer = ((R_Texture*)Engine->moduleResources->GetResource(selectedResourceImage))->buffer;
		ImGui::Image((ImTextureID)texBuffer, ImVec2(imageSize + 20.0f, imageSize + textSize + 24.0f), ImVec2(0, 1), ImVec2(1, 0));
	}

	//If clicked, select resource to display in inspector
	if (ImGui::IsItemClicked())
	{
		selectedResource = resource;

		//TODO: quick workaround to select resources
		Engine->moduleEditor->selectedResources.clear();
		Engine->moduleEditor->selectedResources.push_back(resource);
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
		R_Prefab* modelNode = (R_Prefab*)resource;

		ImGui::SetCursorPos(drawPos + ImVec2(imageSize / 2 + nodeButtonOffset, imageSize / 2 - ImGui::GetFrameHeight() / 2));
		ImGui::ArrowButton("ArrowButton?", modelNode == openModel ? ImGuiDir_Left : ImGuiDir_Right);
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
		{
			openModel = (openModel == modelNode ? nullptr : modelNode);
		}

		if (modelNode == openModel)
		{
			//Draw the resources contained in the prefab
			for (uint i = 0; i < modelNode->containedResources.size(); ++i)
			{
				itemIndex++;
				Resource* containedResource = Engine->moduleResources->GetResource(modelNode->containedResources[i]);
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
	assets = Engine->fileSystem->GetAllFiles("Assets", nullptr, &ignore_ext);
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
		R_Texture* texture = (R_Texture*)Engine->moduleResources->GetResource(resourceIcons[resource->GetType()]);
		textureBuffer = texture->buffer;
	}
	dnd_event->assign("DND_RESOURCE_").append(std::to_string((int)resource->GetType()));

	return textureBuffer;
}