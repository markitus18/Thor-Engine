#ifndef __W_VIEWPORT_H__
#define __W_VIEWPORT_H__

#include "Window.h"
#include "ERenderingFlagsh.h"

#include "ImGuizmo/ImGuizmo.h"
#include "MathGeoLib/src/Math/float3.h"

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
	enum class CameraInputOperation
	{
		NONE = 0,
		SELECTION,
		ORBIT,
		PAN,
		TURN
	};

public:
	WViewport(WindowFrame* parent, const char* name, ImGuiWindowClass* windowClass, int ID);
	~WViewport();

	virtual void PrepareUpdate();
	void Draw() override;
	virtual void OnResize(Vec2 newSize) override;

	//Converts a 2D point in the scene image to a 2D point in the real scene
	Vec2 ScreenToWorld(Vec2 p) const;
	//Converts a 2D point in the real scene to a 2D point in the scene image
	Vec2 WorldToScreen(Vec2 p) const;

	virtual void SetScene(Scene* scene);
	inline const C_Camera* GetCurrentCamera() const { return currentCamera; }
	inline C_Camera* GetCurrentCamera() { return currentCamera; } //Added for scene draw access.. move somewhere else?

	//Camera management methods
	void FocusCameraOnPosition(const float3& position, float distance);
	void SetNewCameraTarget(const float3& new_target);
	void MatchCamera(const C_Camera* camera);

	void SetCameraPosition(float3 position);

	void OnClick(const Vec2& mousePos);
	void PanCamera(float motion_x, float motion_y);
	void OrbitCamera(float motion_x, float motion_y);
	void TurnCamera(float motion_x, float motion_y);
	void ZoomCamera(float zoom);

protected:
	//Renders the scene 
	void DrawScene();
	void DrawToolbarShared();
	virtual void DrawToolbarCustom() {}

private:
	//Handles user input. TODO: Add deltaTime everywhere
	void HandleInput();
	void HandleGizmoUsage();
	void HandleKeyboardInput();

public:
	EViewportViewMode viewMode = EViewportViewMode::LIT;
	EViewportCameraAngle cameraAngle = EViewportCameraAngle::PERSPECTIVE;
	RenderingSettings::RenderingFlags renderingFlags;

	float3 cameraReference; //TODO: Consider moving to C_Camera

private:
	//The scene linked to this viewport
	Scene* scene = nullptr;

	//The current camera used in this viewport. Can be 'perspectiveCamera' or 'orthographicCamera'
	C_Camera* currentCamera = nullptr;

	//Camera set to perspective mode to render the screen
	C_Camera* perspectiveCamera = nullptr;

	//Camera set to orthographic mode to render the screen
	C_Camera* orthographicCamera = nullptr;

	//Bottom-left corner of the scene image in OpenGL space coordinates (0y at the bottom of the screen)
	Vec2 sceneTextureScreenPosition;

	//Top-left corner of the image in ImGui space coordinates (0y at the top of the screen)
	//Defines the origin of the gizmo's effective rectangle
	Vec2 gizmoRectOrigin;

	//The size of the screen texture in the window
	Vec2 textureSize;

	bool cameraSettingsNeedUpdate = false;

	CameraInputOperation cameraInputOperation = CameraInputOperation::NONE;
	Vec2 selectionStartPoint = Vec2(-1, -1);

	//Variables for gizmo's handling
	ImGuizmo::OPERATION gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE gizmoMode = ImGuizmo::MODE::WORLD;
};



#endif // __W_VIEWPORT_H__
