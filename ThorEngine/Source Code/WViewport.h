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
public:
	WViewport(M_Editor* editor, const char* name, ImGuiWindowClass* windowClass, int ID);
	~WViewport() { }

	virtual void PrepareUpdate();
	void Draw() override;
	virtual void OnResize(Vec2 newSize) override;

	//Converts a 2D point in the scene image to a 2D point in the real scene
	Vec2 ScreenToWorld(Vec2 p) const;
	//Converts a 2D point in the real scene to a 2D point in the scene image
	Vec2 WorldToScreen(Vec2 p) const;

	virtual void SetScene(Scene* scene);
	inline const C_Camera* GetCurrentCamera() const { return currentCamera; }

	//Camera management methods
	void CameraLookAt(const float3& position);
	void CenterCameraOn(const float3& position, float distance);
	void SetNewCameraTarget(const float3& new_target);
	void MatchCamera(const C_Camera* camera);

	void SetCameraPosition(float3 position);

	void OnClick(const Vec2& mousePos);
	void MoveCamera_Mouse(float motion_x, float motion_y);
	void PanCamera(float motion_x, float motion_y);
	void OrbitCamera(float motion_x, float motion_y);

	void ZoomCamera(float zoom);

protected:
	//Renders the scene 
	void DrawScene();
	void DrawToolbarShared();
	virtual void DrawToolbarCustom() {}

private:
	//Handles user input
	void HandleInput();
	void HandleGizmoUsage();

public:
	EViewportViewMode viewMode = EViewportViewMode::LIT;
	EViewportCameraAngle cameraAngle = EViewportCameraAngle::PERSPECTIVE;
	RenderingSettings::RenderingFlags renderingFlags;

private:
	//The scene linked to this viewport
	Scene* scene = nullptr;

	//The current camera used in this viewport. Can be 'perspectiveCamera' or 'orthographicCamera'
	C_Camera* currentCamera = nullptr;

	//Camera set to perspective mode to render the screen
	C_Camera* perspectiveCamera = nullptr;

	//Camera set to orthographic mode to render the screen
	C_Camera* orthographicCamera = nullptr;

	Vec2 start_drag;
	Vec2 init_drag_val;

	Vec2 img_corner;
	Vec2 cornerPos;
	bool tabBarHidden;
	bool cameraSettingsNeedUpdate = false;

	Vec2 cursorOffset;
	Vec2 textureSize;

	float3 cameraReference;

	bool draggingOrbit = false;
	bool draggingPan = false;

	//Variables for gizmo's handling
	ImGuizmo::OPERATION gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE gizmoMode = ImGuizmo::MODE::WORLD;
};



#endif // __W_VIEWPORT_H__
