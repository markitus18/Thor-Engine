#include "W_MainToolbar.h"

#include "Engine.h"
#include "M_SceneManager.h"
#include "Scene.h"

#include "Time.h"

#include "ImGui/imgui.h"

W_MainToolbar::W_MainToolbar(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID) : Window(parent, GetName(), windowClass, ID)
{

}


void W_MainToolbar::Draw()
{
	ImGui::SetNextWindowClass(windowClass);
	if (!ImGui::Begin(windowStrID.c_str(), &active)) { ImGui::End(); return; }

	std::string name = Time::running ? "Stop" : "Play";
	if (ImGui::Button(name.c_str()))
	{
		//TODO: This should be handled through windowOwner->GetScene()
		Scene* currentScene = Engine->sceneManager->mainScene;
		if (Time::running) //TODO: Define a Time for each scene
			Engine->sceneManager->StopSceneSimulation(currentScene);
		else
			Engine->sceneManager->StartSceneSimulation(currentScene);
	}

	ImGui::SameLine();
	std::string name2 = Time::paused ? "Resume" : "Pause";
	if (ImGui::Button(name2.c_str()))
	{
		Time::paused ? Time::Resume() : Time::Pause();
	}

	ImGui::End();
}