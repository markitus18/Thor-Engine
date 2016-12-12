#include "P_Explorer.h"
#include "Application.h"
#include "M_FileSystem.h"
#include "PerfTimer.h"
#include "M_Resources.h"
#include "R_Texture.h"

P_Explorer::P_Explorer()
{
	updateTimer.Start();
	UpdateTree();
	currentNode = assets;
}

P_Explorer::~P_Explorer()
{

}

void P_Explorer::Draw(ImGuiWindowFlags flags)
{
	PerfTimer timer;
	timer.Start();
	if (updateTimer.ReadSec() > updateTime)
	{
		UpdateTree();
		updateTimer.Start();
	}
	double time = timer.ReadMs();

	//Drawing folder tree explorer
	if (active)
	{
		ImGui::SetNextWindowPos(position);
		ImGui::SetNextWindowSize(size);

		if (!ImGui::Begin("Explorer", &active, ImVec2(size), 1.0f, flags))
		{
			ImGui::End();
			return;
		}
		else
		{
			ImGuiTreeNodeFlags default_flags = ImGuiTreeNodeFlags_OpenOnArrow;
			DrawFolderNode(assets, default_flags);
			ImGui::End();
		}
	}

	//Drawing folder explorer
	if (explorerActive)
	{
		ImGui::SetNextWindowPos(explorerPosition);
		ImGui::SetNextWindowSize(explorerSize);

		if (!ImGui::Begin("FolderExplorer", &explorerActive, ImVec2(explorerSize), 1.0f, flags))
		{
			ImGui::End();
			return;
		}
		else
		{
			ImGui::Text(currentNode.path.c_str());
			ImGui::Separator();

			ImGui::BeginChild(5);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(50, 30));

			ImVec2 vec = ImGui::GetCursorScreenPos();
			ImVec2 vec2 = ImGui::GetCursorPos();

			PathNode nextCurrent;
			uint line = 0;

			for (uint i = 0; i < currentNode.children.size(); i++)
			{
				ImGui::PushID(i);

				ImGui::SetCursorPosX(vec2.x + (i - (line * columnsNumber)) * (imageSize + imageSpacingX) + imageSpacingX);
				ImGui::SetCursorPosY(vec2.y + line * (imageSize + imageSpacingY));

				DrawNodeImage(currentNode.children[i]);

				ImGui::SetCursorPosX(vec2.x + (i - line * columnsNumber) * (imageSize + imageSpacingX) + imageSpacingX);
				ImGui::SetCursorPosY(vec2.y + line * (imageSize + imageSpacingY));

				if (explorerSelected == currentNode.children[i])
				{
					ImGui::Image((ImTextureID)selectedBuffer, ImVec2(imageSize, imageSize));
				}

				if (ImGui::IsItemClicked())
				{
					explorerSelected = currentNode.children[i];
				}
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && currentNode.children[i].file == false)
				{
					nextCurrent = currentNode.children[i];
				}

				ImGui::SetCursorPosX(vec2.x + (i - line * columnsNumber) * (imageSize + imageSpacingX) + imageSpacingX);
				ImGui::SetCursorPosY(vec2.y + line * (imageSize + imageSpacingY) + imageSize + textOffset);

				std::string textAdjusted = GetTextAdjusted(currentNode.children[i].localPath.c_str());
				ImGui::Text(textAdjusted.c_str());
			
				if ((i + 1) % columnsNumber == 0)
					line++;

				ImGui::PopID();
			}
			ImGui::PopStyleVar();
			ImGui::EndChild();
			ImGui::End();

			if (nextCurrent.path != "")
				currentNode = nextCurrent;
		}

	}
}

void P_Explorer::UpdatePosition(int screen_width, int screen_height)
{
	position.x = 0;
	position.y = screen_height * (0.60);

	size.x = screen_width * (0.20);
	size.y = screen_height * (1.00) - position.y;

	explorerPosition.x = screen_width * 0.20;
	explorerPosition.y = (screen_height * (0.75));

	explorerSize.y = screen_height - explorerPosition.y;
	explorerSize.x = screen_width * (0.80) - explorerPosition.x;

	columnsNumber = ((explorerSize.x - imageSpacingX) * 10) / 1152;
}

void P_Explorer::DrawFolderNode(const PathNode& node, ImGuiTreeNodeFlags flags)
{
	ImGuiTreeNodeFlags nodeFlags = flags;
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
				DrawFolderNode(node.children[i], flags);
			}
			ImGui::TreePop();
		}	
	}
}

void P_Explorer::DrawNodeImage(const PathNode& node)
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
		else
		{
			ImGui::Image((ImTextureID)fileBuffer, ImVec2(imageSize, imageSize));
		}
	}

}

void P_Explorer::UpdateTree()
{
	std::vector<std::string> ignore_ext;
	ignore_ext.push_back("meta");
	assets = App->fileSystem->GetAllFiles("Assets", nullptr, &ignore_ext);
}

std::string P_Explorer::GetTextAdjusted(const char* text)
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