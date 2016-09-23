
#pragma once
#include "glmath.h"
#include "Color.h"

enum GameObjectTypes
{
	GameObject_Point,
	GameObject_Line,
	GameObject_Plane,
	GameObject_Cube,
	GameObject_Sphere,
	GameObject_Cylinder
};

class GameObject
{
public:

	GameObject();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Scale(float x, float y, float z);
	GameObjectTypes	GetType() const;

public:
	
	Color color;
	mat4x4 transform;
	bool axis,wire;
	bool active = true;
	bool alive = true;

protected:
	GameObjectTypes type;
};

// ============================================
class GO_Cube : public GameObject
{
public :
	GO_Cube();
	GO_Cube(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	vec3 size;
};

// ============================================
class GO_Sphere : public GameObject
{
public:
	GO_Sphere();
	GO_Sphere(float radius);
	void InnerRender() const;
public:
	float radius;
};

// ============================================
class GO_Cylinder : public GameObject
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
class GO_Line : public GameObject
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
class GO_Plane : public GameObject
{
public:
	GO_Plane();
	GO_Plane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	vec3 normal;
	float constant;
};

class GO_Grid : public GameObject
{
public:
	GO_Grid();
	~GO_Grid();
	void InnerRender() const;


};