#include "Application.h"
#include "ModuleUI.h"
#include "ModuleWindow.h"
#include "UI_Console.h"

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

	console = new UI_Console();

	test_color = ImColor(114, 144, 154);
	return true;
}

update_status ModuleUI::Update(float dt)
{
	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	ImGui::ShowTestWindow();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Text("Hello, world!");


	if (ImGui::BeginMainMenuBar())
	{
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(1.0f, 0.8f, 0.8f));
		if (ImGui::BeginMenu("File"))
		{
			//ImGui::PopStyleColor();
			if (ImGui::MenuItem("Exit          ", "Esc"))
			{
				ImGui::EndMenu();
				return UPDATE_STOP;	
			}
			ImGui::EndMenu();

		}
		ImGui::PopStyleColor();
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Console          ", "1", &console->active))
			{
			}
			if (ImGui::MenuItem("Settings         ", "2", &console->active))
			{
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About Thor Engine   "))
			{

			}
			ImGui::Separator();
			if (ImGui::MenuItem("Documentation       "))
			{

			}
			if (ImGui::MenuItem("Download latest     "))
			{

			}
			if (ImGui::MenuItem("Report a bug        "))
			{

			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debugging Tools"))
		{
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (ImGui::Button("Quit", ImVec2(100, 50)))
	{
		return UPDATE_STOP;
	}

	char* button_label = "Open Window";
	if (show_second_window)
	{
		button_label = "Close Window";
	}

	if (ImGui::Button(button_label, ImVec2(100, 50)))
	{
		show_second_window ^= 1;
	}

	if (show_second_window)
	{
		//Setting up window flags
		ImGuiWindowFlags window_flags = 0;
		//Allowing menu bar on the window
		window_flags |= ImGuiWindowFlags_MenuBar;

		//ImGui::SetNextWindowSize(ImVec2(560, 680), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("this is a damn window", &show_second_window, ImVec2(500, 300), 1.0f, window_flags);
		ImGui::Text("Hi there niggz!");

		//Menu Bar
		if (ImGui::BeginMenuBar())
		{
			//Menu Bar -- Menu Button
			if (ImGui::BeginMenu("Menu"))
			{
				ImGui::MenuItem("Disabled button", NULL, false, false);
				ImGui::MenuItem("Menu option 1");
				ImGui::MenuItem("Menu option 2");
				ImGui::MenuItem("Menu option 3");
				ImGui::MenuItem("Menu option 4");
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::MenuItem("Nothing in here, sorry!", NULL, false, false);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		
		ImGui::ColorEdit3("Change Bg color", (float*)&test_color);
		ImGui::End();
	}

	if (console->active)
	{
		console->Draw("Console", &console->active);
	}

	//Change background color, we use "test_color", controllable variable from UI
	glClearColor(test_color.x, test_color.y, test_color.z, test_color.w);

	ImGui::Render();
	return UPDATE_CONTINUE;
}

bool ModuleUI::CleanUp()
{
	if (console)
	{
		delete console;
		console = NULL;
	}

	ImGui_ImplSdlGL3_Shutdown();
	return true;
}

void ModuleUI::Log(const char* input)
{
	if (console != NULL)
		console->AddLog(input);
}
