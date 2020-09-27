#include "W_MainToolbar.h"

#include "Engine.h"
#include "M_SceneManager.h"

#include "Time.h"

#include "ImGui/imgui.h"

W_MainToolbar::W_MainToolbar(M_Editor* editor, ImGuiWindowClass* windowClass, int ID) : Window(editor, GetName(), windowClass, ID)
{

}


void W_MainToolbar::Draw()
{
	ImGui::SetNextWindowClass(windowClass);
	if (!ImGui::Begin(windowStrID.c_str(), &active)) { ImGui::End(); return; }

	std::string name = Time::running ? "Stop" : "Play";
	if (ImGui::Button(name.c_str()))
	{
		Time::running ? Engine->sceneManager->Stop() : Engine->sceneManager->Play();
	}
	ImGui::SameLine();
	std::string name2 = Time::paused ? "Resmue" : "Pause";
	if (ImGui::Button(name2.c_str()))
	{
		Time::paused ? Time::Resume() : Time::Pause();
	}

	ImGui::End();
}