#include "W_Hierarchy.h"

#include "Engine.h"
#include "M_Scene.h"

#include "GameObject.h"

W_Hierarchy::W_Hierarchy(M_Editor* editor) : DWindow(editor, "Hierarchy")
{

}

void W_Hierarchy::Draw()
{
	DrawTree(Engine->scene->GetRoot());
}

void W_Hierarchy::OnResize()
{

}