#include "DWindow.h"

#include "Dock.h"

DWindow::DWindow(M_Editor* editor, std::string name) : editor(editor), name(name)
{

}

DWindow::~DWindow()
{

}

void DWindow::Draw()
{

}

void DWindow::SetParent(Dock* dock)
{
	if (parent != nullptr && parent != dock)
	{
		parent->RemoveChildData(this);
	}
	parent = dock;
	OnResize();
}

void DWindow::SetActive(bool active)
{
	this->active = active;
}

Dock* DWindow::GetParent() const
{
	return parent;
}

bool DWindow::IsActive() const
{
	return active;
}