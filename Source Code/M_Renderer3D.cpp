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
#include "M_Materials.h"
#include "M_Input.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib") /* link Microsoft OpenGL lib   */


//TMP TESTING  TODO: what is this?

#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )



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
		glClear(GL_COLOR_BUFFER_BIT);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s", gluErrorString(error));
			ret = false;
		}
		
		// Blend for transparency
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		GLfloat lmodel_ambient[] = { 1.4, 1.4, 1.4, 1.0 };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		lights[0].Active(true);		

		//GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		//GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

		glShadeModel(GL_SMOOTH);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	}

	OnResize();

	mesh_draw_timer = App->moduleEditor->AddTimer("Mesh draw", "Render");
	particles_draw_timer = App->moduleEditor->AddTimer("Particles draw", "Render");
	box_draw_timer = App->moduleEditor->AddTimer("Box draw", "Render");

	//Creating screenshot camera----------------
	screenshotCamera = new C_Camera(nullptr);

	screenshotCamera->frustum.SetViewPlaneDistances(0.1f, 2000.0f);
	screenshotCamera->frustum.SetPerspective(1.0f, 1.0f);

	screenshotCamera->frustum.SetPos(float3(50, 50, 50));
//	screenshotCamera->frustum.SetFront(float3::unitZ);
//	screenshotCamera->frustum.SetUp(float3::unitY);
	screenshotCamera->Look(float3(0, 0, 0));
	screenshotCamera->frustum.SetUp(-screenshotCamera->frustum.Up());
	screenshotCamera->update_projection = true;
	//-----------------------------------------

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
	lights[0].SetPos(App->camera->GetCamera()->frustum.Pos().x, App->camera->GetCamera()->frustum.Pos().y, App->camera->GetCamera()->frustum.Pos().z);

//	for(uint i = 0; i < MAX_LIGHTS; ++i)
//		lights[i].Render();

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

uint M_Renderer3D::SaveImage(const char* source_file)
{
	SDL_Surface* surface = SDL_GetWindowSurface(App->window->window);
	//Enable surface pixel read-write
	SDL_LockSurface(surface);

	char* pixels = new char[surface->w * surface->h * 3];
	glReadPixels(0, 0, surface->w, surface->h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	
	SDL_UnlockSurface(surface);

	uint64 ret = App->moduleResources->ImportPrefabImage(pixels, source_file, surface->w, surface->h);
	/*
	//Generating IL Texture
	ILuint img;
	ilGenImages(1, &img);
	ilBindImage(img);
	ilTexImage(surface->w, surface->h, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, pixels);

	//Saving file
	ilEnable(IL_FILE_OVERWRITE);
	ilSave(IL_DDS, source_file);
	*/
	
	//ilDeleteImages(1, &img);
	
	return ret;

}

uint M_Renderer3D::SavePrefabImage(GameObject* gameObject)
{
	C_Camera* previous_camera = camera;
	SetActiveCamera(screenshotCamera);

	PreUpdate(0);
	gameObject->DrawResursive(true, false, false, false);
	DrawAllScene();

	SetActiveCamera(previous_camera);

	std::string path = "/Library/PrefabImages/";
	path.append(gameObject->name).append(".dds");
	uint ID = SaveImage(path.c_str());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return ID;
}

void M_Renderer3D::SetDepthBufferEnabled(bool enabled)
{
	depthEnabled = enabled;

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, enabled ? depthBuffer : 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Called before quitting
bool M_Renderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

void M_Renderer3D::GenerateSceneBuffers()
{
	//TODO: move into a function
	//Generating buffers for scene render
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	//Generating texture to render to
	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, App->window->windowSize.x, App->window->windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Generating the depth buffer
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App->window->windowSize.x, App->window->windowSize.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//Configuring frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG("Error creating screen buffer");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void M_Renderer3D::OnResize()
{
	glViewport(0, 0, App->window->windowSize.x, App->window->windowSize.y);
	camera->SetAspectRatio((float)App->window->windowSize.x / (float)App->window->windowSize.y);
	UpdateProjectionMatrix();
	GenerateSceneBuffers();
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
	glUseProgram(0);
	//TODO: Move this into a mesh "prefab" or a renderer method
	//Both draw and input handling
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClearColor(0.278f, 0.278f, 0.278f, 0.278f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	if (drawGrid)
	{

		glLineWidth(1.0f);

		glBegin(GL_LINES);
		glColor4f(0.8f, 0.8f, 0.8f, 0.5f);

		float d = 40.0f;

		for (float i = -d; i <= d; i += 2.0f)
		{
			glVertex3f(i, 0.0f, -d);
			glVertex3f(i, 0.0f, d);
			glVertex3f(-d, 0.0f, i);
			glVertex3f(d, 0.0f, i);
		}

		glEnd();
	}

	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
	{
		drawGrid = !drawGrid;
	}

	App->moduleEditor->StartTimer(mesh_draw_timer);
	DrawAllMeshes();
	App->moduleEditor->ReadTimer(mesh_draw_timer);

	App->moduleEditor->StartTimer(particles_draw_timer);
	DrawAllParticles();
	App->moduleEditor->ReadTimer(particles_draw_timer);

	App->moduleEditor->StartTimer(box_draw_timer);
	DrawAllBox();
	App->moduleEditor->ReadTimer(box_draw_timer);
	
	//TODO: move to another side. Camera call to draw on renderer
	if (App->camera->drawRay)
	{
		App->camera->DrawRay();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.278f, 0.278f, 0.278f, 0.278f);
}

void M_Renderer3D::AddMesh(const float4x4& transform, C_Mesh* mesh, const C_Material* material, bool shaded, bool wireframe, bool selected, bool parentSelected, bool flippedNormals)
{
	meshes.push_back(RenderMesh(transform, mesh, material, shaded, wireframe, selected, parentSelected, flippedNormals));
}

void M_Renderer3D::DrawAllMeshes()
{
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_NORMAL_ARRAY);
//	glUseProgram(3);

	for (uint i = 0; i < meshes.size(); i++)
	{
		DrawMesh_Shader(meshes[i]);
	}
	meshes.clear();
//	glUseProgram(0);
//	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_VERTEX_ARRAY);
}

void M_Renderer3D::DrawMesh(RenderMesh& rMesh)
{
	const R_Mesh* resMesh = (R_Mesh*)rMesh.mesh->GetResource();
	if (resMesh == nullptr) return;

	//Generate buffers for animation mesh
	if (rMesh.mesh->animMesh != nullptr)
	{
		glBindBuffer(GL_ARRAY_BUFFER, rMesh.mesh->animMesh->buffers[R_Mesh::b_vertices]);
		glBufferData(GL_ARRAY_BUFFER, resMesh->buffersSize[R_Mesh::b_vertices] * sizeof(float) * 3, rMesh.mesh->animMesh->vertices, GL_STATIC_DRAW);

		if (resMesh->buffersSize[R_Mesh::b_normals] > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, rMesh.mesh->animMesh->buffers[R_Mesh::b_normals]);
			glBufferData(GL_ARRAY_BUFFER, resMesh->buffersSize[R_Mesh::b_normals] * sizeof(float) * 3, rMesh.mesh->animMesh->normals, GL_STATIC_DRAW);
		}
	}

	glPushMatrix();
	glMultMatrixf((float*)&rMesh.transform);

	glBindBuffer(GL_ARRAY_BUFFER, rMesh.mesh->animMesh ? rMesh.mesh->animMesh->buffers[R_Mesh::b_vertices] :  resMesh->buffers[R_Mesh::b_vertices]);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resMesh->buffers[R_Mesh::b_indices]);


	//Selected wireframe drawing
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

			glBindBuffer(GL_ARRAY_BUFFER, rMesh.mesh->animMesh ? rMesh.mesh->animMesh->buffers[R_Mesh::b_normals] : resMesh->buffers[R_Mesh::b_normals]);
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
				R_Texture* rTex = (R_Texture*)App->moduleResources->GetResource(mat->textureID);
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

void M_Renderer3D::DrawMesh_Shader(RenderMesh& rMesh)
{
	const R_Mesh* resMesh = (R_Mesh*)rMesh.mesh->GetResource();
	if (resMesh == nullptr) return;

	glUseProgram(3);

	//Sending model matrix
	uint modelLoc = glGetUniformLocation(3, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, rMesh.transform.ptr());

	//Sending view matrix
	uint projectionLoc = glGetUniformLocation(3, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, App->camera->GetCamera()->GetOpenGLProjectionMatrix());

	//Sending projection matrix
	uint viewLoc = glGetUniformLocation(3, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->camera->GetCamera()->GetOpenGLViewMatrix());
	
	glBindVertexArray(resMesh->VAO);

	if (rMesh.material)
	{
		R_Material* mat = (R_Material*)rMesh.material->GetResource();
		if (mat->textureID)
		{
			R_Texture* rTex = (R_Texture*)App->moduleResources->GetResource(mat->textureID);
			if (rTex && rTex->buffer != 0)
			{
				glBindTexture(GL_TEXTURE_2D, rTex->buffer);
			}
		}
	}

	glDrawElements(GL_TRIANGLES, resMesh->buffersSize[R_Mesh::b_indices], GL_UNSIGNED_INT, nullptr);

	//Back to default OpenGL state --------------
	if (rMesh.material)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glColor4f(255, 255, 255, 1.0f);
	}
	glFrontFace(GL_CCW);
	glUseProgram(0); //TODO: meshes won't render if the program is cleared later...?
	glBindVertexArray(0);
	//------------------------------------------
}

void M_Renderer3D::AddParticle(const float4x4& transform, uint64 material, float4 color, float distanceToCamera)
{
	particles.insert(std::pair<float, RenderParticle>(distanceToCamera, RenderParticle(transform, material, color)));
}

void M_Renderer3D::DrawAllParticles()
{
	std::map<float, RenderParticle>::reverse_iterator it;
	for (it = particles.rbegin(); it != particles.rend(); ++it)
	{
		DrawParticle((*it).second);
	}
	particles.clear();
}

void M_Renderer3D::DrawParticle(RenderParticle& particle)
{
	glPushMatrix();
	glMultMatrixf((float*)&particle.transform);

	//Binding particle Texture
	//if (R_Material* mat = (R_Material*)App->moduleResources->GetResource(particle.materialID))
	//{
	//	if (mat->textureID)
	//	{
			R_Texture* rTex = (R_Texture*)App->moduleResources->GetResource(particle.materialID);
			if (rTex && rTex->buffer != 0)
			{
				glBindTexture(GL_TEXTURE_2D, rTex->buffer);
			}
		//}
	//}

	glColor4f(particle.color.x, particle.color.y, particle.color.z, particle.color.w);

	//Drawing to tris in direct mode
	glBegin(GL_TRIANGLES);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(.5f, -.5f, .0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-.5f, .5f, .0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-.5f, -.5f, .0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(.5f, -.5f, .0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(.5f, .5f, .0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-.5f, .5f, .0f);

	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
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

void M_Renderer3D::AddLine(const float3 a, const float3 b, const Color& color)
{
	lines.push_back(RenderLine(a, b, color));
}

void M_Renderer3D::DrawAllLines()
{
	for (uint i = 0; i < lines.size(); i++)
	{
		glColor3f(lines[i].color.r, lines[i].color.g, lines[i].color.b);
		glVertex3f(lines[i].start.x, lines[i].start.y, lines[i].start.z);
		glVertex3f(lines[i].end.x, lines[i].end.y, lines[i].end.z);
	}
	glEnd();
}

//Component buffers management -----------------
void M_Renderer3D::LoadBuffers(R_Mesh* mesh)
{
	if (mesh->buffersSize[R_Mesh::b_vertices] > 0)
	{
		glGenBuffers(1, (GLuint*)&mesh->buffers[R_Mesh::b_vertices]);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[R_Mesh::b_vertices]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->buffersSize[R_Mesh::b_vertices] * 3, mesh->vertices, GL_STATIC_DRAW);
	}

	if (mesh->buffersSize[R_Mesh::b_indices] > 0)
	{
		glGenBuffers(1, (GLuint*)&mesh->buffers[R_Mesh::b_indices]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->buffers[R_Mesh::b_indices]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->buffersSize[R_Mesh::b_indices], mesh->indices, GL_STATIC_DRAW);
	}

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