#include "Engine.h"

#include "Gizmos.h"
#include "OpenGL.h"
#include "GameObject.h"

#include "M_Renderer3D.h"
#include "M_Window.h"
#include "M_Resources.h"
#include "I_Materials.h"
#include "M_Input.h"
#include "M_Editor.h"

#include "C_Camera.h"
#include "C_Material.h"
#include "C_Mesh.h"
#include "C_Transform.h"

#include "R_Mesh.h"
#include "R_Material.h"
#include "R_Texture.h"
#include "R_Shader.h"


#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib") /* link Microsoft OpenGL lib   */



M_Renderer3D::M_Renderer3D(bool start_enabled) : Module("Renderer", start_enabled)
{
}

// Destructor
M_Renderer3D::~M_Renderer3D()
{

}

// Called before render is available
bool M_Renderer3D::Init(Config& config)
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(Engine->window->window);
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
		if(SDL_GL_SetSwapInterval(VSYNC) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s", SDL_GetError());

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s", gluErrorString(error));
			ret = false;
		}

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Initialize ModelView Matrix
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
		glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
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

		//GLfloat lmodel_ambient[] = { 1.4, 1.4, 1.4, 1.0 };
		//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

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
	//-----------------------------------------

	return ret;
}

bool M_Renderer3D::Start()
{
	hDefaultTexture.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Defaults/Default Texture.png")->ID);
	hDefaultShader.Set(Engine->moduleResources->FindResourceBase("Engine/Assets/Shaders/Default Shader_PlainLight.shader")->ID);

	return true;
}

// PreUpdate: clear buffer
update_status M_Renderer3D::PreUpdate()
{	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//light 0 on cam pos
	//lights[0].SetPos(Engine->camera->GetCamera()->frustum.Pos().x, Engine->camera->GetCamera()->frustum.Pos().y, Engine->camera->GetCamera()->frustum.Pos().z);

	//for(uint i = 0; i < MAX_LIGHTS; ++i)
	//	lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status M_Renderer3D::PostUpdate()
{
	std::vector<CameraTarget>::iterator it;
	for (it = cameraRenderingTargets.begin(); it != cameraRenderingTargets.end(); ++it)
	{
		DrawTargetCamera(*it);
	}
	cameraRenderingTargets.clear();

	//glViewport(0, 0, Engine->window->windowSize.x, Engine->window->windowSize.y); //TODO: Is this necessary...?*/
	Engine->moduleEditor->Draw();

	SDL_GL_SwapWindow(Engine->window->window);

	return UPDATE_CONTINUE;
}

uint M_Renderer3D::SaveImage(const char* source_file)
{
	SDL_Surface* surface = SDL_GetWindowSurface(Engine->window->window);
	//Enable surface pixel read-write
	SDL_LockSurface(surface);

	char* pixels = new char[surface->w * surface->h * 3];
	glReadPixels(0, 0, surface->w, surface->h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	
	SDL_UnlockSurface(surface);

	//WARNING: DevIL was included here to save the thumbnail image. It should be moved to texture importer

	//uint64 ret = Engine->moduleResources->ImportModelThumbnail(pixels, source_file, surface->w, surface->h);
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
	
	return 0;

}

uint M_Renderer3D::SaveModelThumbnail(GameObject* gameObject)
{
/*
	C_Camera* previous_camera = camera;

	PreUpdate();
	gameObject->DrawResursive(true, false, false, false);
	DrawAllScene();

	SetActiveCamera(previous_camera);

	std::string path = "/Library/Thumbnails/";
	path.append(gameObject->name).append(".dds");
	uint ID = SaveImage(path.c_str());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return ID;
	*/
	return 0;
}

// Called before quitting
bool M_Renderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

void M_Renderer3D::OnResize()
{
	glViewport(0, 0, Engine->window->windowSize.x, Engine->window->windowSize.y);
	//camera->SetAspectRatio((float)Engine->window->windowSize.x / (float)Engine->window->windowSize.y);
}

void M_Renderer3D::DrawTargetCamera(CameraTarget& cameraTarget)
{
	currentCameraTarget = &cameraTarget;
	const C_Camera* cameraComponent = cameraTarget.camera;

	//Set projection matrix from the camera. TODO: Do we really need this? Isn't this done through shader?
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(*cameraComponent->GetOpenGLProjectionMatrix().v);

	//Set view matrix from the camera.  TODO: Do we really need this? Isn't this done through shader?
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(*cameraComponent->GetOpenGLViewMatrix().v);

	if (cameraComponent->viewMode & EViewportViewMode::Wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor4f(0.0f, 0.3f, 0.7f, 1.0f);
	}

	//Set camera's frame buffer as the current target buffer
	glBindFramebuffer(GL_FRAMEBUFFER, cameraComponent->GetFrameBuffer());
	glClearColor(cameraComponent->backgroundColor.r, cameraComponent->backgroundColor.g, cameraComponent->backgroundColor.b, cameraComponent->backgroundColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glViewport(0, 0, cameraComponent->resolution.x, cameraComponent->resolution.y);

	glUseProgram(0);

	DrawAllMeshes(cameraTarget);
	DrawAllParticles(cameraTarget);
	DrawAllBox(cameraTarget);
	DrawAllLines(cameraTarget);

	if (cameraComponent->viewMode & EViewportViewMode::Wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//TODO: Move this into a mesh "prefab" or a renderer method
	//Both draw and input handling
	if (cameraComponent->renderingFlags & ERenderingFlags::Grid)
	{
		glLineWidth(1.0f);
		glDisable(GL_LIGHTING);

		glBegin(GL_LINES);
		glColor4f(0.8f, 0.8f, 0.8f, 0.2f);

		float d = 40.0f;

		for (float i = -d; i <= d; i += 2.0f)
		{
			glVertex3f(i, 0.0f, -d);
			glVertex3f(i, 0.0f, d);
			glVertex3f(-d, 0.0f, i);
			glVertex3f(d, 0.0f, i);
		}

		glEnd();
		glEnable(GL_LIGHTING);
	}
	
	//Clear OpenGL states
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void M_Renderer3D::BeginTargetCamera(const C_Camera* camera, EViewportViewMode::Flags viewMode)
{
	cameraRenderingTargets.push_back(CameraTarget(camera, viewMode));
	currentCameraTarget = &cameraRenderingTargets.back();
}

void M_Renderer3D::EndTargetCamera()
{
	currentCameraTarget = nullptr;
}

void M_Renderer3D::AddMesh(const float4x4& transform, C_Mesh* mesh, const C_Material* material)
{
	if (currentCameraTarget)
		currentCameraTarget->meshes.push_back(RenderMesh(transform, mesh, material));
}

void M_Renderer3D::DrawAllMeshes(CameraTarget& cameraTarget)
{
	std::vector<RenderMesh>::iterator it;
	for (it = cameraTarget.meshes.begin(); it != cameraTarget.meshes.end(); ++it)
	{
		DrawMesh(*it);
	}
	cameraTarget.meshes.clear();
}

void M_Renderer3D::DrawMesh(RenderMesh& rMesh)
{
	const R_Mesh* resMesh = rMesh.mesh->rMeshHandle.Get();
	if (resMesh == nullptr) return;
	if (rMesh.material == nullptr) return;

	const R_Material* mat = rMesh.material->rMaterialHandle.GetID() ? rMesh.material->rMaterialHandle.Get() : hDefaultMaterial.Get();
	const R_Shader* shader = mat->hShader.GetID() ? mat->hShader.Get() : hDefaultShader.Get();
	glUseProgram(shader->shaderProgram);

	const R_Texture* rTex = (currentCameraTarget->camera->viewMode & EViewportViewMode::Lit && mat->hTexture.GetID()) ? mat->hTexture.Get() : hDefaultTexture.Get();
	if (rTex && rTex->buffer != 0)
	{
		glBindTexture(GL_TEXTURE_2D, rTex->buffer);
	}

	Color color = currentCameraTarget->camera->viewMode & EViewportViewMode::Wireframe ? wireframeColor : mat->color;

	uint colorLoc = glGetUniformLocation(shader->shaderProgram, "baseColor");
	glUniform4fv(colorLoc, 1, (GLfloat*)&color);

	//Sending prefab matrix
	uint modelLoc = glGetUniformLocation(shader->shaderProgram, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, rMesh.transform.ptr());

	//Sending view matrix
	uint projectionLoc = glGetUniformLocation(shader->shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, *currentCameraTarget->camera->GetOpenGLProjectionMatrix().v);

	//Sending projection matrix
	uint viewLoc = glGetUniformLocation(shader->shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *currentCameraTarget->camera->GetOpenGLViewMatrix().v);
	
	//Binding vertex array object
	if (rMesh.mesh->animMesh == nullptr)
		glBindVertexArray(resMesh->VAO);
	else
		glBindVertexArray(rMesh.mesh->animMesh->VAO);

	glDrawElements(GL_TRIANGLES, resMesh->buffersSize[R_Mesh::b_indices], GL_UNSIGNED_INT, nullptr);

	//Back to default OpenGL state --------------
	glBindTexture(GL_TEXTURE_2D, 0);
	glFrontFace(GL_CCW);
	glUseProgram(0);
	glBindVertexArray(0);
	//------------------------------------------
}

void M_Renderer3D::AddParticle(const float4x4& transform, R_Material* material, float4 color, float distanceToCamera)
{
	if (currentCameraTarget)
		currentCameraTarget->particles.insert(std::pair<float, RenderParticle>(distanceToCamera, RenderParticle(transform, material, color)));
}

void M_Renderer3D::DrawAllParticles(CameraTarget& cameraTarget)
{
	std::map<float, RenderParticle>::reverse_iterator it;
	for (it = cameraTarget.particles.rbegin(); it != cameraTarget.particles.rend(); ++it)
	{
		DrawParticle((*it).second);
	}
	cameraTarget.particles.clear();
}

void M_Renderer3D::DrawParticle(RenderParticle& particle)
{
	glUseProgram(0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(currentCameraTarget->camera->GetOpenGLProjectionMatrix().ptr());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(currentCameraTarget->camera->GetOpenGLViewMatrix().ptr());

	glPushMatrix();
	glMultMatrixf((float*)&particle.transform);

	//Binding particle Texture
	if (particle.mat)
	{
		if (R_Texture* rTex = particle.mat->hTexture.Get())
		{
			if (rTex && rTex->buffer != 0)
			{
				glBindTexture(GL_TEXTURE_2D, rTex->buffer);
			}
		}
	}

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
	if (currentCameraTarget)
		currentCameraTarget->aabb.push_back(RenderBox<AABB>(&box, color));
}

void M_Renderer3D::AddOBB(const OBB& box, const Color& color)
{
	if (currentCameraTarget)
		currentCameraTarget->obb.push_back(RenderBox<OBB>(&box, color));
}

void M_Renderer3D::AddFrustum(const Frustum& box, const Color& color)
{
	if (currentCameraTarget)
		currentCameraTarget->frustum.push_back(RenderBox<Frustum>(&box, color));
}

void M_Renderer3D::DrawAllBox(CameraTarget& cameraTarget)
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);

	std::vector<RenderBox<AABB>>::iterator aabb_it;
	for (aabb_it = cameraTarget.aabb.begin(); aabb_it != cameraTarget.aabb.end(); ++aabb_it)
	{
		Gizmos::DrawWireBox(*aabb_it->box, aabb_it->color);
	}
	cameraTarget.aabb.clear();

	std::vector<RenderBox<OBB>>::iterator obb_it;
	for (obb_it = cameraTarget.obb.begin(); obb_it != cameraTarget.obb.end(); ++obb_it)
	{
		Gizmos::DrawWireBox(*obb_it->box, (*obb_it).color);
	}
	cameraTarget.obb.clear();

	std::vector<RenderBox<Frustum>>::iterator frustum_it;
	for (frustum_it = cameraTarget.frustum.begin(); frustum_it != cameraTarget.frustum.end(); ++frustum_it)
	{
		Gizmos::DrawWireBox(*frustum_it->box, (*frustum_it).color);
	}
	cameraTarget.frustum.clear();

	glEnd();
	glEnable(GL_LIGHTING);
}

void M_Renderer3D::AddLine(const float3 a, const float3 b, const Color& color)
{
	if (currentCameraTarget)
		currentCameraTarget->lines.push_back(RenderLine(a, b, color));
}

void M_Renderer3D::DrawAllLines(CameraTarget& cameraTarget)
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);

	std::vector<RenderLine>::iterator it;
	for (it = cameraTarget.lines.begin(); it != cameraTarget.lines.end(); ++it)
	{
		glColor3f(it->color.r, it->color.g, it->color.b);
		glVertex3f(it->startPoint.x, it->startPoint.y, it->startPoint.z);
		glVertex3f(it->endPoint.x, it->endPoint.y, it->endPoint.z);
	}

	glEnd();
	glEnable(GL_LIGHTING);

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
	if (mesh->buffersSize[R_Mesh::b_vertices] > 0)
	{
		glDeleteBuffers(1, &mesh->buffers[R_Mesh::b_vertices]);
	}

	if (mesh->buffersSize[R_Mesh::b_indices] > 0)
	{
		glDeleteBuffers(1, &mesh->buffers[R_Mesh::b_indices]);
	}

	if (mesh->buffersSize[R_Mesh::b_normals] > 0)
	{
		glDeleteBuffers(1, &mesh->buffers[R_Mesh::b_normals]);
	}

	if (mesh->buffersSize[R_Mesh::b_tex_coords] > 0)
	{
		glDeleteBuffers(1, &mesh->buffers[R_Mesh::b_tex_coords]);
	}
}

void M_Renderer3D::ReleaseBuffers(R_Texture* texture)
{
	if (texture)
	{
		glDeleteBuffers(1, &texture->buffer);
	}
}