
#pragma once
#include "glmath.h"
#include "Color.h"

enum PrimitiveTypes
{
	Primitive_Empty,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder
};

class Primitive
{
public:

	Primitive();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Scale(float x, float y, float z);
	PrimitiveTypes	GetType() const;

private:
	void			DrawAxis() const;

public:
	
	Color color;
	mat4x4 transform;
	bool axis,wire;
	bool active = true;
	bool alive = true;

protected:
	PrimitiveTypes type;
};

// ============================================
class GO_Cube : public Primitive
{
public :
	GO_Cube();
	GO_Cube(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	vec3 size;
};

// ============================================
class GO_Sphere : public Primitive
{
public:
	GO_Sphere();
	GO_Sphere(float radius);
	void InnerRender() const;
public:
	float radius;
};

// ============================================
class GO_Cylinder : public Primitive
{
public:
	GO_Cylinder();
	GO_Cylinder(float radius, float height);
	void InnerRender() const;
public:
	float radius;
	float height;
};

// ============================================
class GO_Line : public Primitive
{
public:
	GO_Line();
	GO_Line(float x, float y, float z);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
};

// ============================================
class GO_Plane : public Primitive
{
public:
	GO_Plane();
	GO_Plane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	vec3 normal;
	float constant;
};

class GO_Grid : public Primitive
{
public:
	GO_Grid();
	~GO_Grid();
	void InnerRender() const;


};