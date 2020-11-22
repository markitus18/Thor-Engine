#ifndef __W_VIEWPORT_H__
#define __W_VIEWPORT_H__

#include "Window.h"
#include "ERenderingFlagsh.h"
#include "ResourceHandle.h"

#include "ImGuizmo/ImGuizmo.h"
#include "MathGeoLib/src/Math/float3.h"

class Scene;
class C_Camera;
class R_Texture;

struct ImGuiWindowClass;

#define CAMERA_KEYBOARD_MULT 60.0f

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

	virtual void SetScene(Scene* scene);
	inline const C_Camera* GetCurrentCamera() const { return currentCamera; }
	inline C_Camera* GetCurrentCamera() { return currentCamera; } //Added for scene draw access.. move somewhere else?

	// Centers the camera on target position at specified distance. Maintains camera rotation
	void FocusCameraOnPosition(const float3& position, float distance);

	// Changes the camera target and performs a LookAt to it
	void SetNewCameraTarget(const float3& new_target);

	// Copies the specs of the target camera
	void MatchCamera(const C_Camera* camera);

	// Changes the camera position, keeping the reference at the same distance
	void SetCameraPosition(float3 position);

protected:
	// Render the scene texture + handle the user input
	void DrawScene();

	// Draw the part of the toolbar shared in all viewport implementations
	void DrawToolbarShared();

	// Draw a custom toolbar with specific implementations
	virtual void DrawToolbarCustom() {}

private:
	//Converts a 2D point in the scene image to a 2D point in the real scene
	Vec2 ScreenToWorld(Vec2 p) const;
	//Converts a 2D point in the real scene to a 2D point in the scene image
	Vec2 WorldToScreen(Vec2 p) const;

	void HandleInput();
	void HandleGizmoUsage();
	void HandleKeyboardInput();

	// Normalizes the mouse click position and performs a raycast collision test
	void OnClick(const Vec2& mousePos);

	// Moves the camera in its X and Y axis
	void PanCamera(float motion_x, float motion_y);

	// Rotates the camera around the camera reference
	void OrbitCamera(float motion_x, float motion_y);

	// Rotates the camera around itself
	void TurnCamera(float motion_x, float motion_y);

	// Moves the camera forward or backward
	void ZoomCamera(float zoom);

public:
	EViewportViewMode::Flags viewMode = EViewportViewMode::Lit;
	EViewportCameraAngle::Flags cameraAngle = EViewportCameraAngle::Perspective;
	ERenderingFlags::Flags renderingFlags = ERenderingFlags::DefaultEditorFlags;

	float3 cameraReference; //TODO: Consider moving to C_Camera (or remove like Unreal?)
	float cameraSpeed = 1.0f;

private:
	static void BeginToolbarStyle();
	static void EndToolbarStyle();

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

	// Toolbar handling
	bool toolbarCollapsed = false;

	static ResourceHandle<R_Texture> hToolbarCollapseButton;
	static ResourceHandle<R_Texture> hToolbarDisplayButton;
};



#endif // __W_VIEWPORT_H__
