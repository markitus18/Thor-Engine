#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"

#include "SDL\include\SDL_opengl.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
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
	if(context == NULL)
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
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s", gluErrorString(error));
			ret = false;
		}
		
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
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

		glShadeModel(GL_SMOOTH);

#pragma region Indexed_Cube
		//----------------------------------------
//		glGenBuffers(1, (GLuint*) &(vertex_id));
//
//#pragma region vertices
//		vertices = new float
//		[
//			//Bottom vertices
//			0, 0, 0, //0
//			0, 0, 1, //1
//			1, 0, 1, //2
//			1, 0, 0, //3
//
//			//Upper vertices
//			0, 1, 0, //4
//			0, 1, 1, //5
//			1, 1, 1, //6
//			1, 1, 0  //7
//		];
//#pragma endregion vertices
//
//		glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3, vertices, GL_STATIC_DRAW);
//
//		glGenBuffers(1, (GLuint*) &(index_id));
//#pragma region indices
//		index = new uint[
//			//Front
//			0, 4, 3,
//			3, 4, 7,
//
//			//Right
//			0, 1, 5,
//			0, 5, 4,
//
//			//Left
//			3, 2, 7,
//			2, 6, 7,
//
//			//Back
//			1, 2, 6,
//			1, 6, 5,
//
//			//Top
//			4, 5, 7,
//			7, 5, 6,
//
//			//Bottom
//			0, 3, 2,
//			0, 2, 1
//		];
//#pragma endregion indices
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_id);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, index, GL_STATIC_DRAW);
//
#pragma endregion

	tri_vertex = new float[
		-1, 0, 0,
		1, 0, 0,
		0, 0, 1];

	tri_index = new uint[0, 1, 2];
	//Triangle test

	glGenBuffers(1, (GLuint*) &(tri_vertex_id));
	glBindBuffer(GL_ARRAY_BUFFER, tri_vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, tri_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*) &(tri_index_id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tri_index_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 3, tri_index, GL_STATIC_DRAW);

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);


	}

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

#pragma region Indexed_Cube
	//glEnableClientState(GL_VERTEX_ARRAY);

	//glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);

	////glEnableClientState(GL_INDEX_ARRAY);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_id);
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	////glDisableClientState(GL_INDEX_ARRAY);

	//glDisableClientState(GL_VERTEX_ARRAY);
#pragma endregion

	//glEnableClientState(GL_VERTEX_ARRAY);


	//glBindBuffer(GL_ARRAY_BUFFER, tri_vertex_id);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tri_index_id);
	//glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);

	////glDisableClientState(GL_INDEX_ARRAY);
	//glDisableClientState(GL_VERTEX_ARRAY);


	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	glVertex3f(1, 1, 0);
	glVertex3f(0, 1, 0);

	glEnd();

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
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
