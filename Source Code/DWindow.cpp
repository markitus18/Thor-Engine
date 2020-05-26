#include "DWindow.h"

#include "Config.h"

DWindow::DWindow(M_Editor* editor, std::string name) : editor(editor), name(name)
{

}

DWindow::~DWindow()
{

}

void DWindow::Draw()
{

}

void DWindow::SetActive(bool active)
{
	this->active = active;
}

void DWindow::SaveConfig(Config& config)
{ 
	config.SetBool("Show Debug Info", showDebugInfo);
}

void DWindow::LoadConfig(Config& config)
{
	showDebugInfo = config.GetBool("Show Debug Info", false);
}