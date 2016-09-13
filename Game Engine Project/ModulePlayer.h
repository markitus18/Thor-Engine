#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "p2List.h"


struct PhysVehicle3D;

#define MAX_ACCELERATION 5000.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 60000.0f
#define MAX_SPEED 15.0f
# define TURRET_SPEED 0.1f
# define CANON_SPEED 0.05f
#define MAX_BULLETS 10
#define BULLET_SPEED 60.0f
#define BULLET_RADIUS 0.5f

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void Reset();

public:
	unsigned int looseMusic;
	unsigned int shootMusic;
	unsigned int cheerMusic;

	uint lifes;

	PhysVehicle3D* vehicle;
	float turn;
	float rightAcceleration;
	float leftAcceleration;
	float brake;

	p2List<PhysBody3D*> bullets;
};