#include "W_Console.h"

#include "ImGui/imgui.h"

W_Console::W_Console(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID) : Window(parent, GetName(), windowClass, ID)
{
	ClearLog();
}

void W_Console::Draw()
{
	ImGui::SetNextWindowClass(windowClass);
	if (!ImGui::Begin(windowStrID.c_str(), &active)) { ImGui::End(); return; }

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6)); // Tighten spacing

	for (uint i = 0; i < items.size(); i++)
	{
		const char* item = items[i];

		ImVec4 col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		if (strstr(item, "[error]"))
			col = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);

		else if (strstr(item, "[warning]"))
			col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Text, col);
		ImGui::TextUnformatted(item);
		ImGui::PopStyleColor();
	}

	if (scrollToBottom)
		ImGui::SetScrollHere(1.0f);

	ImGui::PopStyleVar();

	scrollToBottom = false;

	ImGui::End();
}

void W_Console::ClearLog()
{
	for (int i = 0; i < items.size(); i++)
		free(items[i]);
	items.clear();
	scrollToBottom = true;
}

void W_Console::AddLog(const char* input)
{
	items.push_back(strdup(input));
	scrollToBottom = true;
}