#include "Globals.h"
#include "Light.h"
#include <gl/GL.h>
//#include <gl/GLU.h>

Light::Light() : ref(-1), on(false), position(0.0f, 0.0f, 0.0f)
{}

void Light::Init()
{
	glLightfv(ref, GL_AMBIENT, &ambient);
	glLightfv(ref, GL_DIFFUSE, &diffuse);
}

void Light::SetPos(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Light::Render()
{
	if(on)
	{
		float pos[] = {position.x, position.y, position.z, 1.0f};
		GLfloat lmodel_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
		//glLightfv(ref, GL_POSITION, pos);
	}
}

void Light::Active(bool active)
{
	if(on != active)
	{
		on = !on;

		if(on)
			glEnable(ref);
		else
			glDisable(ref);
	}
}