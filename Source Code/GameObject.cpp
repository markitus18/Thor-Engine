
#include "Globals.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GameObject.h"
#include "glut/glut.h"

#pragma comment (lib, "glut/glut32.lib")

// ------------------------------------------------------------
GameObject::GameObject() : transform(IdentityMatrix), color(White), wire(false), axis(false), type(GameObjectTypes::GameObject_Point)
{}

// ------------------------------------------------------------
GameObjectTypes GameObject::GetType() const
{
	return type;
}

// ------------------------------------------------------------
void GameObject::Render() const
{
	glPushMatrix();
	glMultMatrixf(transform.M);

	if(axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	//if (wire)
	//{
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//}

//	else
//	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	}


	InnerRender();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();
}

// ------------------------------------------------------------
void GameObject::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

// ------------------------------------------------------------
void GameObject::SetPos(float x, float y, float z)
{
	transform.translate(x, y, z);
}

// ------------------------------------------------------------
void GameObject::SetRotation(float angle, const vec3 &u)
{
	transform.rotate(angle, u);
}

// ------------------------------------------------------------
void GameObject::Scale(float x, float y, float z)
{
	transform.scale(x, y, z);
}

// CUBE ============================================
GO_Cube::GO_Cube() : GameObject(), size(1.0f, 1.0f, 1.0f)
{
	type = GameObjectTypes::GameObject_Cube;
}

GO_Cube::GO_Cube(float sizeX, float sizeY, float sizeZ) : GameObject(), size(sizeX, sizeY, sizeZ)
{
	type = GameObjectTypes::GameObject_Cube;
}

void GO_Cube::InnerRender() const
{	
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-sx, -sy, sz);
	glVertex3f( sx, -sy, sz);
	glVertex3f( sx,  sy, sz);
	glVertex3f(-sx,  sy, sz);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f( sx, -sy, -sz);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx,  sy, -sz);
	glVertex3f( sx,  sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(sx, -sy,  sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx,  sy, -sz);
	glVertex3f(sx,  sy,  sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, -sy,  sz);
	glVertex3f(-sx,  sy,  sz);
	glVertex3f(-sx,  sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-sx, sy,  sz);
	glVertex3f( sx, sy,  sz);
	glVertex3f( sx, sy, -sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f( sx, -sy, -sz);
	glVertex3f( sx, -sy,  sz);
	glVertex3f(-sx, -sy,  sz);

	glEnd();
}

// SPHERE ============================================
GO_Sphere::GO_Sphere() : GameObject(), radius(1.0f)
{
	type = GameObjectTypes::GameObject_Sphere;
}

GO_Sphere::GO_Sphere(float radius) : GameObject(), radius(radius)
{
	type = GameObjectTypes::GameObject_Sphere;
}

void GO_Sphere::InnerRender() const
{
	glutSolidSphere(radius, 25, 25);
}


// CYLINDER ============================================
GO_Cylinder::GO_Cylinder() : GameObject(), radius(1.0f), height(1.0f)
{
	type = GameObjectTypes::GameObject_Cylinder;
}

GO_Cylinder::GO_Cylinder(float radius, float height) : GameObject(), radius(radius), height(height)
{
	type = GameObjectTypes::GameObject_Cylinder;
}

void GO_Cylinder::InnerRender() const
{
	int n = 30;

	// Cylinder Bottom
	glBegin(GL_POLYGON);
	
	for(int i = 360; i >= 0; i -= (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder Top
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for(int i = 0; i <= 360; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(height * 0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder "Cover"
	glBegin(GL_QUAD_STRIP);
	for(int i = 0; i < 480; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians

		glVertex3f(height*0.5f,  radius * cos(a), radius * sin(a) );
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a) );
	}
	glEnd();
}

// LINE ==================================================
GO_Line::GO_Line() : GameObject(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = GameObjectTypes::GameObject_Line;
}

GO_Line::GO_Line(float x, float y, float z) : GameObject(), origin(0, 0, 0), destination(x, y, z)
{
	type = GameObjectTypes::GameObject_Line;
}

void GO_Line::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
}

// PLANE ==================================================
GO_Plane::GO_Plane() : GameObject(), normal(0, 1, 0), constant(1)
{
	type = GameObjectTypes::GameObject_Plane;
}

GO_Plane::GO_Plane(float x, float y, float z, float d) : GameObject(), normal(x, y, z), constant(d)
{
	type = GameObjectTypes::GameObject_Plane;
}

void GO_Plane::InnerRender() const
{

	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, -1.0f);

	glVertex3f(-1, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, -1, 1);
	glVertex3f(-1, -1, 1);

	glEnd();
}

GO_Grid::GO_Grid()
{

}
GO_Grid::~GO_Grid()
{

}

void GO_Grid::InnerRender() const
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 20.0f;
	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}