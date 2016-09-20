#include "UI_Console.h"
#include <stdlib.h>

UI_Console::UI_Console()
{
	ClearLog();
}

UI_Console::~UI_Console()
{
	ClearLog();
}

void UI_Console::ClearLog()
{
	for (int i = 0; i < items.Size; i++)
		free(items[i]);
	items.clear();
	scrollToBottom = true;
}

void UI_Console::AddLog(const char* input)
{
	items.push_back(strdup(input));
	scrollToBottom = true;
}

void UI_Console::Draw(const char* title, bool* p_open)
{
	if (ImGui::Begin("Console", &active, ImVec2(500, 300), 1.0f, 0))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, -10)); // Tighten spacing

		ImColor col = ImColor(0.6f, 0.6f, 1.0f, 1.0f);

		for (int i = 0; i < items.Size; i++)
		{
			const char* item = items[i];
			ImGui::PushStyleColor(0, col);
			ImGui::TextUnformatted(item);
			ImGui::PopStyleColor();

		}

		if (scrollToBottom)
			ImGui::SetScrollHere(1.0f);

		ImGui::PopStyleVar();

		scrollToBottom = false;

		ImGui::End();
	}
}