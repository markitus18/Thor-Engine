#include "Application.h"
#include "ModuleUI.h"
#include "ModuleWindow.h"

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl_gl3.h"

ModuleUI::ModuleUI(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleUI::~ModuleUI()
{}

bool ModuleUI::Init()
{
	ImGui_ImplSdlGL3_Init(App->window->window);

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = "imgui.ini";

	test_color = ImColor(114, 144, 154);
	return true;
}

update_status ModuleUI::Update(float dt)
{
	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	ImGui::ShowTestWindow();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Text("Hello, world!");



	if (ImGui::Button("Quit", ImVec2(100, 50)))
	{
		return UPDATE_STOP;
	}

	if (ImGui::Button("Open Window", ImVec2(100, 50)))
	{
		show_second_window ^= 1;
	}

	if (show_second_window)
	{
		ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Another Window", &show_second_window, ImVec2(0.f, 0.f), 0.5f);
		ImGui::Text("Hi there niggz!");
		ImGui::BeginMenu("Menu");
		ImGui::ColorEdit3("Color bitches :D", (float*)&test_color);
		ImGui::End();
	}

	//Change background color, we use "test_color", controllable variable from UI
	glClearColor(test_color.x, test_color.y, test_color.z, test_color.w);

	ImGui::Render();
	return UPDATE_CONTINUE;
}

bool ModuleUI::CleanUp()
{
	ImGui_ImplSdlGL3_Shutdown();
	return true;
}
