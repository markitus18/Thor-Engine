#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"
#include "C_Camera.h"
#include "C_Material.h"
#include "C_Mesh.h"
#include "Gizmos.h"
#include "ModuleEditor.h"
#include "OpenGL.h"
#include "ModuleImport.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module("Renderer", start_enabled)
{
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
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

		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

	//	glEnable(GL_BLEND);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		lights[0].Active(true);

		glShadeModel(GL_SMOOTH);
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

#pragma region Cubes
//#pragma region Direct_texture
//
//	GLubyte checkImage[40][40][4];
//	for (int i = 0; i < 40; i++)
//	{
//		for (int j = 0; j < 40; j++)
//		{
//			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
//			checkImage[i][j][0] = (GLubyte)c;
//			checkImage[i][j][1] = (GLubyte)c;
//			checkImage[i][j][2] = (GLubyte)c;
//			checkImage[i][j][3] = (GLubyte)255;
//		}
//	}
//
//
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	glGenTextures(1, &image_texture);
//	glBindTexture(GL_TEXTURE_2D, image_texture);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 40, 40, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//#pragma endregion
//
//#pragma region Array_Cube
//
//	float array_cube_vertices[] = 
//	{
//		//Front face
//		0, 0, 0,
//		0, 1, 0,
//		1, 0, 0,
//
//		1, 0, 0,
//		0, 1, 0,
//		1, 1, 0,
//
//		//Right face
//		0, 0, 0,
//		0, 0, 1,
//		0, 1, 1,
//
//		0, 0, 0,
//		0, 1, 1,
//		0, 1, 0,
//
//		//Back face
//		0, 0, 1,
//		1, 1, 1,
//		0, 1, 1,
//
//		0, 0, 1,
//		1, 0, 1,
//		1, 1, 1,
//
//		//Left face
//		1, 0, 0,
//		1, 1, 0,
//		1, 0, 1,
//
//		1, 0, 1,
//		1, 1, 0,
//		1, 1, 1,
//
//		//Upper face
//		0, 1, 0,
//		0, 1, 1,
//		1, 1, 0,
//
//		1, 1, 0,
//		0, 1, 1,
//		1, 1, 1,
//
//		//Bottom face
//		0, 0, 0,
//		1, 0, 0,
//		1, 0, 1,
//
//		0, 0, 0,
//		1, 0, 1,
//		0, 0, 1
//	};
//
//	for (int i = 0; i < 108; i += 3)
//	{
//		array_cube_vertices[i] += 3;
//	}
//	glGenBuffers(1, (GLuint*)&array_cube_id);
//	glBindBuffer(GL_ARRAY_BUFFER, array_cube_id);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 108, array_cube_vertices, GL_STATIC_DRAW);
//#pragma endregion
//
//#pragma region Array Index Cube
//
//	cube_vertices[0] = 0;		cube_vertices[1] = 0;		cube_vertices[2] = 0;
//	cube_vertices[3] = 0;		cube_vertices[4] = 0;		cube_vertices[5] = 1;
//	cube_vertices[6] = 1;		cube_vertices[7] = 0;		cube_vertices[8] = 1;
//	cube_vertices[9] = 1;		cube_vertices[10] = 0;		cube_vertices[11] = 0;
//	cube_vertices[12] = 0;		cube_vertices[13] = 1;		cube_vertices[14] = 0;
//	cube_vertices[15] = 0;		cube_vertices[16] = 1;		cube_vertices[17] = 1;
//	cube_vertices[18] = 1;		cube_vertices[19] = 1;		cube_vertices[20] = 1;
//	cube_vertices[21] = 1;		cube_vertices[22] = 1;		cube_vertices[23] = 0;
//
//	for (int i = 0; i < 24; i += 3)
//	{
//		cube_vertices[i] -= 3;
//	}
//
//	glGenBuffers(1, (GLuint*)&index_cube_vertex_id);
//	glBindBuffer(GL_ARRAY_BUFFER, index_cube_vertex_id);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, cube_vertices, GL_STATIC_DRAW);
//
//
//	//Front face
//	cube_indices[0] = 0;		cube_indices[1] = 4;		cube_indices[2] = 3;
//	cube_indices[3] = 3;		cube_indices[4] = 4;		cube_indices[5] = 7;
//
//	//Right face
//	cube_indices[6] = 0;		cube_indices[7] = 1;		cube_indices[8] = 5;
//	cube_indices[9] = 0;		cube_indices[10] = 5;		cube_indices[11] = 4;
//
//	//Back face
//	cube_indices[12] = 1;		cube_indices[13] = 2;		cube_indices[14] = 6;
//	cube_indices[15] = 1;		cube_indices[16] = 6;		cube_indices[17] = 5;
//	
//	//Left face
//	cube_indices[18] = 2;		cube_indices[19] = 3;		cube_indices[20] = 7;
//	cube_indices[21] = 2;		cube_indices[22] = 7;		cube_indices[23] = 6;
//	
//	//Upper face
//	cube_indices[24] = 7;		cube_indices[25] = 4;		cube_indices[26] = 5;
//	cube_indices[27] = 7;		cube_indices[28] = 5;		cube_indices[29] = 6;
//	
//	//Bottom face
//	cube_indices[30] = 0;		cube_indices[31] = 3;		cube_indices[32] = 2;
//	cube_indices[33] = 0;		cube_indices[34] = 2;		cube_indices[35] = 1;
//
//	glGenBuffers(1, (GLuint*)&index_cube_index_id);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_cube_index_id);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, cube_indices, GL_STATIC_DRAW);
//#pragma endregion
//
//#pragma region Textured_Cube
//
//	//Loading vertex data
//	texture_vertices[0] = 0;		texture_vertices[1] = 1;		texture_vertices[2] = 0;		//0
//	texture_vertices[3] = 1;		texture_vertices[4] = 1;		texture_vertices[5] = 1;		//1
//	texture_vertices[6] = 1;		texture_vertices[7] = 1;		texture_vertices[8] = 1;		//2
//	texture_vertices[9] = 0;		texture_vertices[10] = 1;		texture_vertices[11] = 1;		//3
//	texture_vertices[12] = 0;		texture_vertices[13] = 1;		texture_vertices[14] = 1;		//4
//	texture_vertices[15] = 1;		texture_vertices[16] = 1;		texture_vertices[17] = 0;		//5
//	texture_vertices[18] = 1;		texture_vertices[19] = 0;		texture_vertices[20] = 1;		//6
//	texture_vertices[21] = 1;		texture_vertices[22] = 0;		texture_vertices[23] = 1;		//7
//	texture_vertices[24] = 0;		texture_vertices[25] = 0;		texture_vertices[26] = 1;		//8
//	texture_vertices[27] = 0;		texture_vertices[28] = 0;		texture_vertices[29] = 1;		//9
//	texture_vertices[30] = 1;		texture_vertices[31] = 0;		texture_vertices[32] = 0;		//10
//	texture_vertices[33] = 1;		texture_vertices[34] = 0;		texture_vertices[35] = 0;		//11
//	texture_vertices[36] = 1;		texture_vertices[37] = 0;		texture_vertices[38] = 0;		//12
//	texture_vertices[39] = 0;		texture_vertices[40] = 0;		texture_vertices[41] = 0;		//13
//	texture_vertices[42] = 0;		texture_vertices[43] = 0;		texture_vertices[44] = 0;		//14
//	texture_vertices[45] = 0;		texture_vertices[46] = 0;		texture_vertices[47] = 0;		//15
//
//	for (int i = 0; i < 48; i += 3)
//	{
//		texture_vertices[i] -= 6;
//	}
//
//	glGenBuffers(1, (GLuint*)&texture_vertex_id);
//	glBindBuffer(GL_ARRAY_BUFFER, texture_vertex_id);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 48, texture_vertices, GL_STATIC_DRAW);
//
//	//Loading UV data
//	texture_UV[0] = 1;	texture_UV[1] = 1;
//	texture_UV[2] = 0;	texture_UV[3] = 0;
//	texture_UV[4] = 1;	texture_UV[5] = 1;
//	texture_UV[6] = 1;	texture_UV[7] = 0;
//	texture_UV[8] = 0;	texture_UV[9] = 1;
//	texture_UV[10] = 0;	texture_UV[11] = 1;
//	texture_UV[12] = 1;	texture_UV[13] = 0;
//	texture_UV[14] = 0;	texture_UV[15] = 0;
//	texture_UV[16] = 0;	texture_UV[17] = 0;
//	texture_UV[18] = 1;	texture_UV[19] = 0;
//	texture_UV[20] = 0;	texture_UV[21] = 0;
//	texture_UV[22] = 1;	texture_UV[23] = 1;
//	texture_UV[24] = 0;	texture_UV[25] = 1;
//	texture_UV[26] = 0;	texture_UV[27] = 1;
//	texture_UV[28] = 1;	texture_UV[29] = 0;
//	texture_UV[30] = 1;	texture_UV[31] = 1;
//
//	glGenBuffers(1, (GLuint*)&texture_UV_id);
//	glBindBuffer(GL_ARRAY_BUFFER, texture_UV_id);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 32, texture_UV, GL_STATIC_DRAW);
//
//	//Loading index data
//	//Front face
//	texture_indices[0] = 10;		texture_indices[1] = 14;	texture_indices[2] = 0;
//	texture_indices[3] = 10;		texture_indices[4] = 0;		texture_indices[5] = 5;
//
//	//Right face
//	texture_indices[6] = 13;		texture_indices[7] = 8;		texture_indices[8] = 3;
//	texture_indices[9] = 13;		texture_indices[10] = 3;	texture_indices[11] = 0;
//
//	//Left face
//	texture_indices[12] = 6;		texture_indices[13] = 11;	texture_indices[14] = 5;
//	texture_indices[15] = 6;		texture_indices[16] = 5;	texture_indices[17] = 1;
//
//	//Back face
//	texture_indices[18] = 8;		texture_indices[19] = 6;	texture_indices[20] = 2;
//	texture_indices[21] = 8;		texture_indices[22] = 2;	texture_indices[23] = 4;
//
//	//Upper face
//	texture_indices[24] = 5;	texture_indices[25] = 3;	texture_indices[26] = 1;
//	texture_indices[27] = 5;	texture_indices[28] = 0;	texture_indices[29] = 3;
//
//	//Bottom face
//	texture_indices[30] = 15;	texture_indices[31] = 12;	texture_indices[32] = 7;
//	texture_indices[33] = 15;	texture_indices[34] = 7;	texture_indices[35] = 9;
//
//	glGenBuffers(1, (GLuint*)&texture_index_id);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texture_index_id);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, texture_indices, GL_STATIC_DRAW);
//#pragma endregion
#pragma endregion
	mesh_draw_timer = App->moduleEditor->AddTimer("Mesh draw", "Render");
	box_draw_timer = App->moduleEditor->AddTimer("Box draw", "Render");

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
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

#pragma region Cubes
//#pragma region Direct-Mode Cube
//
//	glBindTexture(GL_TEXTURE_2D, image_texture);
//	glBegin(GL_TRIANGLES);
//
//
//	//Front face
//	glTexCoord2f(0.0f, 0.0f);
//	glVertex3f(0, 0, 0);
//	glTexCoord2f(0.0f, 1.0f);
//	glVertex3f(0, 1, 0);
//	glTexCoord2f(1.0f, 0.0f);
//	glVertex3f(1, 0, 0);
//
//	glTexCoord2f(1.0f, 0.0f);
//	glVertex3f(1, 0, 0);
//	glTexCoord2f(0.0f, 1.0f);
//	glVertex3f(0, 1, 0);
//	glTexCoord2f(1.0f, 1.0f);
//	glVertex3f(1, 1, 0);
//	
//	//Right face
//	glTexCoord2f(0.0f, 0.0f);
//	glVertex3f(0, 0, 0);
//	glTexCoord2f(1.0f, 1.0f);
//	glVertex3f(0, 1, 1);
//	glTexCoord2f(1.0f, 0.0f);
//	glVertex3f(0, 1, 0);
//
//	glTexCoord2f(0.0f, 0.0f);
//	glVertex3f(0, 0, 0);
//	glTexCoord2f(0.0f, 1.0f);
//	glVertex3f(0, 0, 1);
//	glTexCoord2f(1.0f, 1.0f);
//	glVertex3f(0, 1, 1);
//
//	//Back face
//	glTexCoord2f(0.0f, 0.0f);
//	glVertex3f(0, 0, 1);
//	glTexCoord2f(1.0f, 1.0f);
//	glVertex3f(1, 1, 1);
//	glTexCoord2f(0.0f, 1.0f);
//	glVertex3f(0, 1, 1);
//
//	glTexCoord2f(0.0f, 0.0f);
//	glVertex3f(0, 0, 1);
//	glTexCoord2f(1.0f, 0.0f);
//	glVertex3f(1, 0, 1);
//	glTexCoord2f(1.0f, 1.0f);
//	glVertex3f(1, 1, 1);
//
//	//Left face
//	glTexCoord2f(0.0f, 0.0f);
//	glVertex3f(1, 0, 0);
//	glTexCoord2f(1.0f, 0.0f);
//	glVertex3f(1, 1, 0);
//	glTexCoord2f(0.0f, 1.0f);
//	glVertex3f(1, 0, 1);
//
//	glTexCoord2f(1.0f, 0.0f);
//	glVertex3f(1, 1, 0);
//	glTexCoord2f(1.0f, 1.0f);
//	glVertex3f(1, 1, 1);
//	glTexCoord2f(0.0f, 1.0f);
//	glVertex3f(1, 0, 1);
//
//	//Upper face
//	glTexCoord2f(0.0f, 0.0f);
//	glVertex3f(0, 1, 0);
//	glTexCoord2f(0.0f, 1.0f);
//	glVertex3f(0, 1, 1);
//	glTexCoord2f(1.0f, 0.0f);
//	glVertex3f(1, 1, 0);
//
//	glTexCoord2f(1.0f, 0.0f);
//	glVertex3f(1, 1, 0);
//	glTexCoord2f(0.0f, 1.0f);
//	glVertex3f(0, 1, 1);
//	glTexCoord2f(1.0f, 1.0f);
//	glVertex3f(1, 1, 1);
//
//	//Bottom face
//	glTexCoord2f(0.0f, 0.0f);
//	glVertex3f(0, 0, 0);
//	glTexCoord2f(1.0f, 0.0f);
//	glVertex3f(1, 0, 0);
//	glTexCoord2f(1.0f, 1.0f);
//	glVertex3f(1, 0, 1);
//
//	glTexCoord2f(0.0f, 0.0f);
//	glVertex3f(0, 0, 0);
//	glTexCoord2f(1.0f, 1.0f);
//	glVertex3f(1, 0, 1);
//	glTexCoord2f(0.0f, 1.0f);
//	glVertex3f(0, 0, 1);
//
//	glEnd();
//	glBindTexture(GL_TEXTURE_2D, 0);
//#pragma endregion
//
//#pragma region Array Cube
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glBindBuffer(GL_ARRAY_BUFFER, array_cube_id);
//	glVertexPointer(3, GL_FLOAT, 0, nullptr);
//
//	glColor4f(0.0, 1, 0, 1);
//	glDrawArrays(GL_TRIANGLES, 0, 36);
//	glColor4f(1, 1, 1, 1);
//
//	glDisableClientState(GL_VERTEX_ARRAY);
//#pragma endregion
//
//#pragma region Array Index Cube
//
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glBindBuffer(GL_ARRAY_BUFFER, index_cube_vertex_id);
//	glVertexPointer(3, GL_FLOAT, 0, nullptr);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_cube_index_id);
//
//	glColor4f(0.0, 0, 1, 1);
//	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
//	glColor4f(1, 1, 1, 1);
//
//	glDisableClientState(GL_VERTEX_ARRAY);
//
//#pragma endregion
//
//#pragma region Textured_Cube
//
//	if (!lenaON)
//	{
//		lenna_texture = App->moduleImport->LoadIMG("Game/Lenna.png");
//		lenaON = true;
//	}
//
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glBindBuffer(GL_ARRAY_BUFFER, texture_vertex_id);
//	glVertexPointer(3, GL_FLOAT, 0, nullptr);
//
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//	glBindBuffer(GL_ARRAY_BUFFER, texture_UV_id);
//	glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texture_index_id);
//
//	glBindTexture(GL_TEXTURE_2D, lenna_texture);
//	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
//
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//	glDisableClientState(GL_VERTEX_ARRAY);
//	glBindTexture(GL_TEXTURE_2D, 0);
//#pragma endregion
#pragma endregion

	DrawAllScene();
	App->moduleEditor->Draw();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	window_width = width;
	window_height = height;

	glViewport(0, 0, width, height);
	camera->SetAspectRatio((float)width / (float)height);
	UpdateProjectionMatrix();
}

void ModuleRenderer3D::UpdateProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glLoadMatrixf((GLfloat*)camera->GetOpenGLProjectionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::SetActiveCamera(C_Camera* camera)
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

C_Camera* ModuleRenderer3D::GetActiveCamera()
{
	return camera;
}

void ModuleRenderer3D::SetCullingCamera(C_Camera* camera)
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

void ModuleRenderer3D::DrawAllScene()
{
	App->moduleEditor->StartTimer(mesh_draw_timer);
	DrawAllMeshes();
	App->moduleEditor->ReadTimer(mesh_draw_timer);

	App->moduleEditor->StartTimer(box_draw_timer);
	DrawAllBox();
	App->moduleEditor->ReadTimer(box_draw_timer);
}

void ModuleRenderer3D::AddMesh(float4x4 transform, C_Mesh* mesh, C_Material* material, bool shaded, bool wireframe, bool selected, bool parentSelected, bool flippedNormals)
{
	meshes.push_back(RenderMesh(transform, mesh, material, shaded, wireframe, selected, parentSelected, flippedNormals));
}

void ModuleRenderer3D::DrawAllMeshes()
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

void ModuleRenderer3D::DrawMesh(const RenderMesh& mesh)
{
	glPushMatrix();
	glMultMatrixf((float*)&mesh.transform);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.mesh->id_vertices);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.mesh->id_indices);

	if (mesh.selected || mesh.parentSelected || mesh.wireframe)
	{
		if (mesh.selected)
		{
			glColor3f(0.71, 0.78, 0.88);
			glLineWidth(1);
		}

		else if (mesh.parentSelected)
		{
			glColor3f(0.51, 0.58, 0.68);
		}

		else if (mesh.wireframe)
		{
			glColor3f(0, 0, 0);
		}

		glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, mesh.mesh->num_indices, GL_UNSIGNED_INT, nullptr);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_LIGHTING);
		glLineWidth(1);
		glColor4f(1, 1, 1, 1);
	}

	if (mesh.shaded)
	{
		if (mesh.mesh->num_normals > 0)
		{
			if (mesh.flippedNormals)
			{
				glFrontFace(GL_CW);
			}

			glBindBuffer(GL_ARRAY_BUFFER, mesh.mesh->id_normals);
			glNormalPointer(GL_FLOAT, 0, nullptr);
		}

		if (mesh.mesh->num_tex_coords > 0)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, mesh.mesh->id_tex_coords);
			glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
		}

		if (mesh.material)
		{
			if (mesh.material->texture_id)
			{
				glBindTexture(GL_TEXTURE_2D, mesh.material->texture_id);
			}
			glColor4f(mesh.material->color.r, mesh.material->color.g, mesh.material->color.b, mesh.material->color.a);
		}

		glDrawElements(GL_TRIANGLES, mesh.mesh->num_indices, GL_UNSIGNED_INT, nullptr);

		//Back to default OpenGL state --------------
		if (mesh.material)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			glColor4f(255, 255, 255, 1.0f);
		}

		if (mesh.mesh->num_tex_coords > 0)
		{
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		glFrontFace(GL_CCW);
		//------------------------------------------
	}

	glPopMatrix();
}

void ModuleRenderer3D::AddAABB(const AABB& box, const Color& color)
{
	aabb.push_back(RenderBox<AABB>(&box, color));
}

void ModuleRenderer3D::AddOBB(const OBB& box, const Color& color)
{
	obb.push_back(RenderBox<OBB>(&box, color));
}

void ModuleRenderer3D::AddFrustum(const Frustum& box, const Color& color)
{
	frustum.push_back(RenderBox<Frustum>(&box, color));
}

void ModuleRenderer3D::DrawAllBox()
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
void ModuleRenderer3D::LoadBuffers(C_Mesh* mesh)
{
	glGenBuffers(1, (GLuint*)&mesh->id_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertices * 3, mesh->vertices, GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*)&mesh->id_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_indices, mesh->indices, GL_STATIC_DRAW);

	if (mesh->num_normals > 0)
	{
		glGenBuffers(1, (GLuint*)&mesh->id_normals);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_normals * 3, mesh->normals, GL_STATIC_DRAW);
	}

	if (mesh->num_tex_coords > 0)
	{
		glGenBuffers(1, (GLuint*)&mesh->id_tex_coords);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->id_tex_coords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_tex_coords * 2, mesh->tex_coords, GL_STATIC_DRAW);
	}
}

void ModuleRenderer3D::LoadBuffers(C_Material* material)
{

}

void ModuleRenderer3D::ReleaseBuffers(C_Mesh* mesh)
{

}

void ModuleRenderer3D::ReleaseBuffers(C_Material* material)
{
	if (material->texture_id)
	{
		glDeleteBuffers(1, &material->texture_id);

	}
}
//----------------------------------------------