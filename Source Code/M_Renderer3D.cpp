#include "Application.h"
#include "M_Renderer3D.h"
#include "M_Camera3D.h"
#include "M_Window.h"
#include "C_Camera.h"
#include "C_Material.h"
#include "C_Mesh.h"
#include "R_Mesh.h"
#include "Gizmos.h"
#include "M_Editor.h"
#include "OpenGL.h"
#include "M_Import.h"
#include "GameObject.h"
#include "R_Material.h"
#include "R_Texture.h"
#include "M_Resources.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib") /* link Microsoft OpenGL lib   */

M_Renderer3D::M_Renderer3D(bool start_enabled) : Module("Renderer", start_enabled)
{
}

// Destructor
M_Renderer3D::~M_Renderer3D()
{}

// Called before render is available
bool M_Renderer3D::Init(Config& config)
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == nullptr)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s", SDL_GetError());
		ret = false;
	}
	
	GLenum error = glewInit();

	if (error != GL_NO_ERROR)
	{
		LOG("Error initializing glew library! %s", SDL_GetError());
		ret = false;
	}

	if(ret == true)
	{
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s", gluErrorString(error));
			ret = false;
		}



		int monitor_screen_width = GetSystemMetrics(SM_CXSCREEN);
		int monitor_screen_height = GetSystemMetrics(SM_CYSCREEN);
		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.278f, 0.278f, 0.278f, 0.278f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s", gluErrorString(error));
			ret = false;
		}
		
		// Blend for transparency
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		GLfloat lmodel_ambient[] = { 1.4, 1.4, 1.4, 1.0 };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

		//lights[0].ref = GL_LIGHT0;
		//lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		//lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		//lights[0].SetPos(0.0f, 0.0f, 2.5f);
		//lights[0].Init();
		
		//GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		//GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

	//	glEnable(GL_BLEND);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//lights[0].Active(true);

		glShadeModel(GL_SMOOTH);
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	mesh_draw_timer = App->moduleEditor->AddTimer("Mesh draw", "Render");
	box_draw_timer = App->moduleEditor->AddTimer("Box draw", "Render");

	return ret;
}

// PreUpdate: clear buffer
update_status M_Renderer3D::PreUpdate(float dt)
{
	if (camera->update_projection)
	{
		UpdateProjectionMatrix();
		camera->update_projection = false;
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(camera->GetOpenGLViewMatrix());

	// light 0 on cam pos
	//lights[0].SetPos(App->camera->GetCamera()->frustum.pos.x, App->camera->GetCamera()->frustum.pos.y, App->camera->GetCamera()->frustum.pos.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

#pragma endregion
	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status M_Renderer3D::PostUpdate(float dt)
{
	DrawAllScene();
	App->moduleEditor->Draw();

	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool M_Renderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}


void M_Renderer3D::OnResize(int width, int height)
{
	window_width = width;
	window_height = height;

	glViewport(0, 0, width, height);
	camera->SetAspectRatio((float)width / (float)height);
	UpdateProjectionMatrix();
}

void M_Renderer3D::UpdateProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glLoadMatrixf((GLfloat*)camera->GetOpenGLProjectionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void M_Renderer3D::SetActiveCamera(C_Camera* camera)
{
	if (this->camera)
	{
		this->camera->active_camera = false;
	}
	this->camera = camera ? camera : App->camera->GetCamera();
	if (camera)
	{
		camera->active_camera = true;
	}
	UpdateProjectionMatrix();
}

C_Camera* M_Renderer3D::GetActiveCamera()
{
	return camera;
}

void M_Renderer3D::SetCullingCamera(C_Camera* camera)
{
	if (culling_camera)
	{
		culling_camera->culling = false;
	}
	this->culling_camera = camera;
	if (camera)
	{
		camera->culling = true;
	}

}

void M_Renderer3D::DrawAllScene()
{
	App->moduleEditor->StartTimer(mesh_draw_timer);
	DrawAllMeshes();
	App->moduleEditor->ReadTimer(mesh_draw_timer);

	App->moduleEditor->StartTimer(box_draw_timer);
	DrawAllBox();
	App->moduleEditor->ReadTimer(box_draw_timer);
}

void M_Renderer3D::AddMesh(float4x4 transform, const C_Mesh* mesh, const C_Material* material, bool shaded, bool wireframe, bool selected, bool parentSelected, bool flippedNormals)
{
	meshes.push_back(RenderMesh(transform, mesh, material, shaded, wireframe, selected, parentSelected, flippedNormals));
}

void M_Renderer3D::DrawAllMeshes()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	for (uint i = 0; i < meshes.size(); i++)
	{
		DrawMesh(meshes[i]);
	}
	meshes.clear();

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void M_Renderer3D::DrawMesh(const RenderMesh& rMesh)
{
	const R_Mesh* resMesh = (R_Mesh*)rMesh.mesh->GetResource();

	glPushMatrix();
	glMultMatrixf((float*)&rMesh.transform);

	glBindBuffer(GL_ARRAY_BUFFER, resMesh->buffers[R_Mesh::b_vertices]);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resMesh->buffers[R_Mesh::b_indices]);

	if (rMesh.selected || rMesh.parentSelected || rMesh.wireframe)
	{
		if (rMesh.selected)
		{
			glColor3f(0.71, 0.78, 0.88);
			glLineWidth(1);
		}

		else if (rMesh.parentSelected)
		{
			glColor3f(0.51, 0.58, 0.68);
		}

		else if (rMesh.wireframe)
		{
			glColor3f(0, 0, 0);
		}

		glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, resMesh->buffersSize[R_Mesh::b_indices], GL_UNSIGNED_INT, nullptr);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_LIGHTING);
		glLineWidth(1);
		glColor4f(1, 1, 1, 1);
	}

	if (rMesh.shaded)
	{
		if (resMesh->buffersSize[R_Mesh::b_normals] > 0)
		{
			if (rMesh.flippedNormals)
			{
				glFrontFace(GL_CW);
			}

			glBindBuffer(GL_ARRAY_BUFFER, resMesh->buffers[R_Mesh::b_normals]);
			glNormalPointer(GL_FLOAT, 0, nullptr);
		}

		if (resMesh->buffersSize[R_Mesh::b_tex_coords] > 0)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, resMesh->buffers[R_Mesh::b_tex_coords]);
			glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
		}

		if (rMesh.material)
		{
			R_Material* mat = (R_Material*)rMesh.material->GetResource();
			if (mat->textureID)
			{
				R_Texture* rTex = (R_Texture*)App->moduleResources->GetResource(mat->textureID, Resource::TEXTURE);
				if (rTex && rTex->buffer != 0)
				{
					glBindTexture(GL_TEXTURE_2D, rTex->buffer);
				}
			}
			glColor4f(mat->color.r, mat->color.g, mat->color.b, mat->color.a);
		}

		glDrawElements(GL_TRIANGLES, resMesh->buffersSize[R_Mesh::b_indices], GL_UNSIGNED_INT, nullptr);

		//Back to default OpenGL state --------------
		if (rMesh.material)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			glColor4f(255, 255, 255, 1.0f);
		}

		if (resMesh->buffersSize[R_Mesh::b_tex_coords] > 0)
		{
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		glFrontFace(GL_CCW);
		//------------------------------------------
	}

	glPopMatrix();
}

void M_Renderer3D::AddAABB(const AABB& box, const Color& color)
{
	aabb.push_back(RenderBox<AABB>(&box, color));
}

void M_Renderer3D::AddOBB(const OBB& box, const Color& color)
{
	obb.push_back(RenderBox<OBB>(&box, color));
}

void M_Renderer3D::AddFrustum(const Frustum& box, const Color& color)
{
	frustum.push_back(RenderBox<Frustum>(&box, color));
}

void M_Renderer3D::DrawAllBox()
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);

	for (uint i = 0; i < aabb.size(); i++)
	{
		Gizmos::DrawWireBox(*aabb[i].box, aabb[i].color);
	}
	aabb.clear();

	for (uint i = 0; i < obb.size(); i++)
	{
		Gizmos::DrawWireBox(*obb[i].box, obb[i].color);
	}
	obb.clear();

	for (uint i = 0; i < frustum.size(); i++)
	{
		Gizmos::DrawWireBox(*frustum[i].box, frustum[i].color);
	}
	frustum.clear();

	glEnd();
	glEnable(GL_LIGHTING);
}

//Component buffers management -----------------
void M_Renderer3D::LoadBuffers(R_Mesh* mesh)
{
	glGenBuffers(1, (GLuint*)&mesh->buffers[R_Mesh::b_vertices]);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[R_Mesh::b_vertices]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->buffersSize[R_Mesh::b_vertices] * 3, mesh->vertices, GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*)&mesh->buffers[R_Mesh::b_indices]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->buffers[R_Mesh::b_indices]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->buffersSize[R_Mesh::b_indices], mesh->indices, GL_STATIC_DRAW);

	if (mesh->buffersSize[R_Mesh::b_normals] > 0)
	{
		glGenBuffers(1, (GLuint*)&mesh->buffers[R_Mesh::b_normals]);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[R_Mesh::b_normals]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->buffersSize[R_Mesh::b_normals] * 3, mesh->normals, GL_STATIC_DRAW);
	}

	if (mesh->buffersSize[R_Mesh::b_tex_coords] > 0)
	{
		glGenBuffers(1, (GLuint*)&mesh->buffers[R_Mesh::b_tex_coords]);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[R_Mesh::b_tex_coords]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->buffersSize[R_Mesh::b_tex_coords] * 2, mesh->tex_coords, GL_STATIC_DRAW);
	}
}

void M_Renderer3D::LoadBuffers(C_Material* material)
{

}

void M_Renderer3D::ReleaseBuffers(R_Mesh* mesh)
{

}

void M_Renderer3D::ReleaseBuffers(R_Texture* texture)
{
	if (texture)
	{
		glDeleteBuffers(1, &texture->buffer);

	}
}

void M_Renderer3D::OnRemoveGameObject(GameObject* gameObject)
{
	for (std::vector<RenderMesh>::iterator it = meshes.begin(); it != meshes.end();)
	{
		if ((*it).mesh->gameObject == gameObject)
			meshes.erase(it);
		else
			it++;
	}
}
//----------------------------------------------