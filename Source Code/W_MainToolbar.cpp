#include "W_MainToolbar.h"

#include "Engine.h"
#include "M_Scene.h"
#include "M_Editor.h"

#include "Time.h"

#include "ImGui/imgui.h"

W_MainToolbar::W_MainToolbar(M_Editor* editor) : Window(editor, "Toolbar")
{

}


void W_MainToolbar::Draw()
{
	ImGui::SetNextWindowClass(editor->normalWindowClass);
	if (!ImGui::Begin(name.c_str(), &active)) { ImGui::End(); return; }

	std::string name = Time::running ? "Stop" : "Play";
	if (ImGui::Button(name.c_str()))
	{
		Time::running ? Engine->scene->Stop() : Engine->scene->Play();
	}
	ImGui::SameLine();
	std::string name2 = Time::paused ? "Resmue" : "Pause";
	if (ImGui::Button(name2.c_str()))
	{
		Time::paused ? Time::Resume() : Time::Pause();
	}

	ImGui::End();
}