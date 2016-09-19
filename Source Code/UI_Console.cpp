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
	buffer.clear();
	scrollToBottom = true;
}

void UI_Console::AddLog(const char* input)
{
	buffer.append(input);
	scrollToBottom = true;
}

void UI_Console::Draw(const char* title, bool* p_open)
{
	if (ImGui::Begin("Console", p_open, ImVec2(500, 300), 1.0f, 0))
	{
		ImColor col = ImColor(0.6f, 0.6f, 1.0f, 1.0f);
		ImGui::PushStyleColor(0, col);

		ImGui::TextUnformatted(buffer.begin());
		ImGui::PopStyleColor();

		if (scrollToBottom)
			ImGui::SetScrollHere(1.0f);

		scrollToBottom = false;

		ImGui::End();
	}
}