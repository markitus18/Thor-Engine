#include "W_About.h"

#include "ImGui/imgui.h"

W_About::W_About(M_Editor* editor) : Window(editor, "About")
{

}


void W_About::Draw()
{
	if (!ImGui::Begin("About Thor Engine", &active, ImGuiWindowFlags_NoResize)) { ImGui::End(); return; }

	ImGui::Text("v0.4-alpha");
	ImGui::Separator();
	ImGui::Text("By Marc Garrigo for educational purposes.");
	ImGui::Text("Thor Engine is licensed under Public Domain, see LICENSE for more information.");
	ImGui::End();
}