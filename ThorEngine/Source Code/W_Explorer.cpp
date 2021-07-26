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
#include "R_Model.h"
#include "R_Folder.h"

W_Explorer::W_Explorer(WindowFrame* parent, ImGuiWindowClass* windowClass, ImGuiWindowClass* explorerWindowClass, int ID) : Window(parent, GetName(), windowClass, ID),
																														 explorerWindowClass(explorerWindowClass)
{
	updateTimer.Start();
	UpdateTree();

	uint64 assetsID = Engine->moduleResources->FindResourceBase("Assets")->ID;
	Engine->moduleEditor->hExplorerFolder.Set(assetsID);
	hAssetsFolder.Set(assetsID);

	hEngineAssetsFolder.Set(Engine->moduleResources->FindResourceBase("Engine/Assets")->ID);

	hResourceIcons[ResourceType::FOLDER].Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/FolderIcon.png")->ID);
	hResourceIcons[ResourceType::MESH].Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/FileIcon.png")->ID);
	hResourceIcons[ResourceType::TEXTURE].Set(uint64(0));
	hResourceIcons[ResourceType::MATERIAL].Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/MaterialIcon.png")->ID);
	hResourceIcons[ResourceType::ANIMATION].Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/AnimationIcon.png")->ID);
	hResourceIcons[ResourceType::ANIMATOR_CONTROLLER].Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/AnimatorIcon.png")->ID);
	hResourceIcons[ResourceType::MODEL].Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/SceneIcon.png")->ID);
	hResourceIcons[ResourceType::PARTICLESYSTEM].Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/ParticlesIcon.png")->ID);
	hResourceIcons[ResourceType::SHADER].Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/ShaderIcon.png")->ID);
	hResourceIcons[ResourceType::MAP].Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/ThorIcon.png")->ID);
	hResourceHighlight.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Icons/SelectedIcon.png")->ID);
}

void W_Explorer::Draw()
{
	ImGui::SetNextWindowClass(windowClass);
	if (!ImGui::Begin(windowStrID.c_str(), &active)) { ImGui::End(); return; }

	ImVec2 winSize = ImGui::GetWindowSize();
	if (winSize.x != windowSize.x || winSize.y != windowSize.y)
		OnResize(Vec2(winSize.x, winSize.y));

	if (updateTimer.ReadSec() > updateTime)
	{
		UpdateTree();
		updateTimer.Start();
	}

	std::string dockStrID = windowStrID + "_DockSpace";
	ImGuiID dockspace_id = ImGui::GetID(dockStrID.c_str());
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoSplit | ImGuiDockNodeFlags_NoTabBar, explorerWindowClass);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowClass(explorerWindowClass);
	std::string childWindowName = std::string("Explorer_Tree") + ("##") + std::to_string(ID);
	ImGui::Begin(childWindowName.c_str(), nullptr, windowFlags);
	DrawFolderNode(assets);
	DrawFolderNode(engineAssets);
	ImGui::End();

	ImGui::SetNextWindowClass(explorerWindowClass);
	childWindowName = std::string("Explorer_Folder") + ("##") + std::to_string(ID);
	ImGui::Begin(childWindowName.c_str(), false, windowFlags);
	DrawSelectedFolderContent();
	ImGui::End();

	ImGui::SetNextWindowClass(explorerWindowClass);
	childWindowName = std::string("Explorer_Toolbar") + ("##") + std::to_string(ID);
	ImGui::Begin(childWindowName.c_str(), false, windowFlags);
	//TODO: Add toolbar content for explorer
	ImGui::End();

	ImGui::End();
}

void W_Explorer::OnResize(Vec2 newSize)
{
	windowSize = newSize;
	columnsNumber = ((newSize.x * 0.8f ) * 10.0f) / 1152.0f;
}

void W_Explorer::LoadLayout_Default()
{
	ImGui::Begin(windowStrID.c_str());

	std::string dockStrID = windowStrID + "_DockSpace";
	ImGuiID dock_space_C = ImGui::GetID(dockStrID.c_str());
	ImGui::DockSpace(dock_space_C, ImVec2(0.0f, 0.0f), 0, explorerWindowClass);

	// Split the new dock in two pieces (top and bottom)
	ImGuiID dock_space_C_Top, dock_space_C_Bottom;
	ImGui::DockBuilderSplitNode(dock_space_C, ImGuiDir_Up, 0.10f, &dock_space_C_Top, &dock_space_C_Bottom);

	std::string windowName;
	windowName = std::string("Explorer_Toolbar") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), dock_space_C_Top);
	ImGui::DockBuilderGetNode(dock_space_C_Top)->WantHiddenTabBarToggle = true;

	// Split the bottom dock in two more pieces (left and right)
	ImGuiID leftExplorerSpace_id, rightExplorerSpace_id;
	ImGui::DockBuilderSplitNode(dock_space_C_Bottom, ImGuiDir_Left, 0.15f, &leftExplorerSpace_id, &rightExplorerSpace_id);

	// Attach windows to the different nodes (code irrelevant from this point)
	windowName = std::string("Explorer_Tree") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), leftExplorerSpace_id);
	ImGui::DockBuilderGetNode(leftExplorerSpace_id)->LocalFlags |= ImGuiDockNodeFlags_AutoHideTabBar;

	windowName = std::string("Explorer_Folder") + ("##") + std::to_string(ID);
	ImGui::DockBuilderDockWindow(windowName.c_str(), rightExplorerSpace_id);
	ImGui::DockBuilderGetNode(rightExplorerSpace_id)->LocalFlags |= ImGuiDockNodeFlags_AutoHideTabBar;

	ImGui::End();
}
void W_Explorer::DrawFolderNode(PathNode& node)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (node.IsLastFolder() == true)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;
		nodeFlags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	if (Engine->moduleEditor->hExplorerFolder.GetID() != 0 && node.path == Engine->moduleEditor->GetCurrentExplorerDirectory())
	{
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	//If node folder has something inside
	if (node.isFile == false)
	{
		bool open = ImGui::TreeNodeEx(node.localPath.c_str(), nodeFlags, node.localPath.c_str());
		if (ImGui::IsItemClicked())
		{
			SetCurrentFolder(Engine->moduleResources->FindResourceBase(node.path.c_str())->ID);
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

void W_Explorer::DrawResourceImage(const ResourceBase& base)
{
	std::string dnd_event("NONE");
	uint textureID = GetTextureFromResource(base, &dnd_event);

	ImGui::Image((ImTextureID)textureID, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		if (dnd_event != "NONE")
		{
			ImGui::SetDragDropPayload(dnd_event.c_str(), &base.ID, sizeof(uint64));
			ImGui::Text(base.name.c_str());
			ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(0.0f, 20.0f));
			ImGui::Image((ImTextureID)textureID, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::EndDragDropSource();
	}

	glBindTexture(GL_TEXTURE_2D, 0); //Soo... this needs to be done in order to reset the texture buffer
}

void W_Explorer::DrawSelectedFolderContent()
{
	ImGui::BeginChild("ExplorerFolder", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_NoScrollbar);

	ImGui::Text(Engine->moduleEditor->GetCurrentExplorerDirectory());
	ImGui::Separator();

	ImGui::BeginChild("ImagesChild");
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(50, 30));

	ImVec2 windowCursorPosition = ImGui::GetCursorPos();
	uint itemIndex = 0;

	R_Folder* currentFolder = Engine->moduleEditor->hExplorerFolder.Get();
	for (uint i = 0; i < currentFolder->baseData->containedResources.size(); ++i)
	{
		const ResourceBase& childBase = *Engine->moduleResources->GetResourceBase(currentFolder->baseData->containedResources[i]);
		DrawResourceItem(childBase, itemIndex, windowCursorPosition);
		itemIndex++;

	}
	if (nextCurrentFolderID != 0)
	{
		SetCurrentFolder(nextCurrentFolderID);
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();

	ImGui::EndChild();
}

void W_Explorer::DrawResourceItem(const ResourceBase& base, uint& itemIndex, ImVec2 windowCursorPos)
{
	if (columnsNumber == 0) return; //No space to draw any resource, exit function

	uint row = itemIndex / columnsNumber;
	uint column = itemIndex - (row * columnsNumber);

	ImVec2 drawPos(windowCursorPos.x + column * (imageSize + imageSpacingX) + imageSpacingX,
		windowCursorPos.y + row * (imageSize + imageSpacingY) + topMarginOffset);

	ImGui::PushID(row * column);
	ImGui::SetCursorPos(drawPos);

	DrawResourceImage(base);

	//Draw selection image
	if (Engine->moduleEditor->hSelectedResource.GetID() == base.ID)
	{
		ImGui::SetCursorPos(drawPos - ImVec2(topMarginOffset, topMarginOffset));
		float textSize = ImGui::GetTextLineHeight();
		uint texBuffer = hResourceHighlight.Get()->buffer;
		ImGui::Image((ImTextureID)texBuffer, ImVec2(imageSize + 20.0f, imageSize + textSize + 24.0f), ImVec2(0, 1), ImVec2(1, 0));
	}

	//If clicked, select resource to display in inspector
	if (ImGui::IsItemClicked())
	{
		Engine->moduleEditor->hSelectedResource.Set(base.ID);
	}
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
	{
		HandleResourceDoubleClick(base);
	}

	ImVec2 textPos = drawPos + ImVec2(0.0f, imageSize + textOffset);
	ImGui::SetCursorPos(textPos);
	ImGui::Text(GetTextAdjusted(base.name.c_str()).c_str());

	//Drawing node Button (if it's a model)
	if (base.type == ResourceType::MODEL)
	{
		ImGui::SetCursorPos(drawPos + ImVec2(imageSize / 2 + nodeButtonOffset, imageSize / 2 - ImGui::GetFrameHeight() / 2));
		ImGui::ArrowButton("ArrowButton?", base.ID == openModelID ? ImGuiDir_Left : ImGuiDir_Right);
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
		{
			SetOpenModel(base.ID != openModelID ? base.ID : 0);
		}

		if (base.ID == openModelID)
		{
			//Draw the resources contained in the model
			for (uint i = 0; i < base.containedResources.size(); ++i)
			{
				itemIndex++;
				const ResourceBase& childBase = *Engine->moduleResources->GetResourceBase(base.containedResources[i]);
				DrawResourceItem(childBase, itemIndex, windowCursorPos);
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
	engineAssets = Engine->fileSystem->GetAllFiles("Engine/Assets", nullptr, &ignore_ext);
}

std::string W_Explorer::GetTextAdjusted(const char* text) const
{
	std::string newText = text;
	uint textLenght = newText.size();

	const char* text_end = text + textLenght;
	float textSizeX = ImGui::CalcTextSize(text, text_end, false, 0).x;
	float dotsSizeX = ImGui::CalcTextSize("...", nullptr, false, 0).x;

	if (textSizeX > imageSize + 15.0f)
	{
		uint charSize = textSizeX / textLenght;
		uint newLenght = ((imageSize + 15.0f - dotsSizeX) / charSize) - 1;
		newText = newText.substr(0, newLenght);
		newText.append("...");
	}
	return newText;
}

void W_Explorer::HandleResourceDoubleClick(const ResourceBase& base)
{
	if (base.type == ResourceType::FOLDER)
		nextCurrentFolderID = base.ID;
	else
		Engine->moduleEditor->OpenWindowFromResource(base.ID);
}

uint W_Explorer::GetTextureFromResource(const ResourceBase& base, std::string* dnd_event)
{
	uint textureBuffer = 0;
	if (base.type == ResourceType::TEXTURE)
	{
		std::map<uint64, ResourceHandle<R_Texture>>::iterator it = texturesInFolder.find(base.ID);
		if (it != texturesInFolder.end())
			textureBuffer = it->second.Get()->buffer;
		else if ((it = texturesInModel.find(base.ID)) != texturesInModel.end())
			textureBuffer = it->second.Get()->buffer;
	}
	else
	{
		textureBuffer = hResourceIcons[base.type].Get()->buffer;
	}
	dnd_event->assign("DND_RESOURCE_").append(std::to_string((int)base.type));

	return textureBuffer;
}

void W_Explorer::SetCurrentFolder(uint64 folderID)
{
	nextCurrentFolderID = 0;

	//Freeing the textures held from the previous folder
	std::map<uint64, ResourceHandle<R_Texture>>::iterator it;
	for (it = texturesInFolder.begin(); it != texturesInFolder.end(); ++it)
	{
		it->second.Free();
	}
	texturesInFolder.clear();
	SetOpenModel(0);

	//Adding textures from the new folder
	Engine->moduleEditor->hExplorerFolder.Set(folderID);
	R_Folder* newFolder = Engine->moduleEditor->hExplorerFolder.Get();
	for (uint i = 0; i < newFolder->baseData->containedResources.size(); ++i)
	{
		const ResourceBase& base = *Engine->moduleResources->GetResourceBase(newFolder->baseData->containedResources[i]);
		if (base.type == ResourceType::TEXTURE)
		{
			texturesInFolder[base.ID] = ResourceHandle<R_Texture>(base.ID);
		}
	}
}

void W_Explorer::SetOpenModel(uint64 modelID)
{
	//Freeing the textures held from the previous model
	std::map<uint64, ResourceHandle<R_Texture>>::iterator it;
	for (it = texturesInModel.begin(); it != texturesInModel.end(); ++it)
	{
		it->second.Free();
	}
	texturesInModel.clear();

	if (const ResourceBase* base = Engine->moduleResources->GetResourceBase(modelID))
	{
		for (uint i = 0; i < base->containedResources.size(); ++i)
		{
			const ResourceBase& childBase = *Engine->moduleResources->GetResourceBase(base->containedResources[i]);
			if (childBase.type == ResourceType::TEXTURE)
			{
				texturesInModel[childBase.ID] = ResourceHandle<R_Texture>(childBase.ID);
			}
		}
	}
	openModelID = modelID;
}