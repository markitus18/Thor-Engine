#include "Window.h"

#include "Config.h"

Window::Window(M_Editor* editor, std::string name, ImGuiWindowClass* windowClass) : editor(editor), name(name), windowClass(windowClass)
{

}

Window::~Window()
{

}

void Window::Draw()
{

}

void Window::SetActive(bool active)
{
	this->active = active;
}

void Window::SaveConfig(Config& config)
{ 
	config.SetBool("Show Debug Info", showDebugInfo);
}

void Window::LoadConfig(Config& config)
{
	showDebugInfo = config.GetBool("Show Debug Info", false);
}