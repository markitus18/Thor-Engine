#ifndef __W_VIEWPORT_H__
#define __W_VIEWPORT_H__

#include "Window.h"
#include "ERenderingFlagsh.h"

class Scene;
class C_Camera;

struct ImGuiWindowClass;

enum class EViewportViewMode
{
	NONE		= 0,
	LIT			= 1 << 0,
	WIREFRAME	= 1 << 1,
	UNLIT		= 1 << 2,
};

enum class EViewportCameraAngle
{
	NONE		= 0,
	PERSPECTIVE = 1 << 0,
	TOP			= 1 << 1,
	BOTTOM		= 1 << 2,
	LEFT		= 1 << 3,
	RIGHT		= 1 << 4,
	FRONT		= 1 << 5,
	BACK		= 1 << 6,
};

class WViewport : public Window
{
public:
	WViewport(M_Editor* editor, const char* name, ImGuiWindowClass* windowClass, int ID);
	~WViewport() { }

	virtual void Draw() override;
	virtual void OnResize(Vec2 newSize) override;

	inline const C_Camera* GetCurrentCamera() const { return currentCamera; }

protected:
	//Renders the scene 
	void DrawScene();
	void DrawToolbarShared();
	virtual void DrawToolbarCustom();

public:
	EViewportViewMode viewMode = EViewportViewMode::LIT;
	EViewportCameraAngle cameraAngle = EViewportCameraAngle::PERSPECTIVE;
	RenderingFlags renderingFlags;

private:
	//The scene linked to this viewport
	Scene* scene = nullptr;

	//The current camera used in this viewport. Can be 'perspectiveCamera' or 'orthographicCamera'
	C_Camera* currentCamera = nullptr;

	//Camera set to perspective mode to render the screen
	C_Camera* perspectiveCamera = nullptr;

	//Camera set to orthographic mode to render the screen
	C_Camera* orthographicCamera = nullptr;
};


#endif // __W_VIEWPORT_H__
