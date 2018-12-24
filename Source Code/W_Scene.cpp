#include "W_Scene.h"

#include "ImGui\imgui.h"
#include "Glew/include/glew.h"

#include "Dock.h"

#include "Application.h"
#include "M_Window.h"
#include "M_Renderer3D.h"

W_Scene::W_Scene(M_Editor* editor) : DWindow(editor, "Scene")
{

}

void W_Scene::Draw()
{
	ImGui::SetCursorPos(ImVec2(img_offset.x, img_offset.y));
	img_corner = Vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y) - Vec2(0, img_size.y);
	ImGui::Image((ImTextureID)App->renderer3D->renderTexture, ImVec2(img_size.x, img_size.y), ImVec2(0, 1), ImVec2(1, 0));
}

void W_Scene::OnResize()
{
	//Getting window size - some margins - tab size (25) - separator (7)
	win_size = Vec2(parent->size.x, parent->size.y) - Vec2(10, 42);

	//Calculating the image size according to the window size.
	img_size = App->window->windowSize;// -Vec2(0.0f, 25.0f); //Removing the tab area
	if (img_size.x > win_size.x)
	{
		img_size /= (img_size.x / win_size.x);
	}
	if (img_size.y > win_size.y)
	{
		img_size /= (img_size.y / win_size.y);
	}
	img_offset = Vec2(win_size.x - img_size.x, win_size.y - img_size.y) / 2 + Vec2(5.0f, 12.0f);

}

//Converts a 2D point in the scene image to a 2D point in the real scene
Vec2 W_Scene::ScreenToWorld(Vec2 p) const
{
	Vec2 ret = p - img_corner;
	ret = ret / img_size * App->window->windowSize;
	return ret;
}

//Converts a 2D point in the real scene to a 2D point in the scene image
Vec2 W_Scene::WorldToScreen(Vec2 p) const
{
	Vec2 ret = p / App->window->windowSize * img_size;
	ret += img_corner;
	return ret;
}

//Handles user input
void W_Scene::HandleInput()
{

}