#include "Window.h"

#include "Config.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

Window::Window(WindowFrame* parent, std::string name, ImGuiWindowClass* windowClass, int ID) : parentFrame(parent), name(name), windowClass(windowClass), ID(ID)
{
	windowStrID = name + ("##") + std::to_string(ID);
}

Window::~Window()
{

}

void Window::OnResize(Vec2 newSize)
{
	windowSize = newSize;
	isTabBarHidden = !(ImGui::GetCurrentWindow()->DockNode && ImGui::GetCurrentWindow()->DockNode->IsHiddenTabBar());
}

void Window::SaveConfig(Config& config)
{ 
	config.SetBool("Show Debug Info", showDebugInfo);
}

void Window::LoadConfig(Config& config)
{
	showDebugInfo = config.GetBool("Show Debug Info", false);
}