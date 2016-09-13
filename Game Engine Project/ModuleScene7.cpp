#include "Globals.h"
#include "Application.h"
#include "ModuleScene7.h"
#include "Primitive.h"
#include "PhysBody3D.h"


ModuleScene7::ModuleScene7(Application* app, bool start_enabled) : ModuleScene(app, start_enabled)
{
}

void ModuleScene7::LoadPositions()
{
	neutralNum = 8;
	neutralPositions = new vec4[neutralNum];

	//HALL
	//Behind Wall
	neutralPositions[0] = { -20, 0, -20, 1 };
	neutralPositions[1] = { 0, 0, -20, 1 };
	neutralPositions[2] = { 20, 0, -20, 1 };
	neutralPositions[3] = { 20, 0, 0, 1 };
	neutralPositions[4] = { 20, 0, 20, 1 };
	neutralPositions[5] = { 0, 0, 20, 1 };
	neutralPositions[6] = { -20, 0, 20, 1 };
	neutralPositions[7] = { -20, 0, 0, 1 };

	enemyNum = 8;
	enemyPositions = new vec4[enemyNum];

	enemyPositions[0] = { -20, 4, -20, 1 };
	enemyPositions[1] = { 0, 6, -20, 1 };
	enemyPositions[2] = { 20, 8, -20, 1 };
	enemyPositions[3] = { 20, 6, 0, 1 };
	enemyPositions[4] = { 20, 4, 20, 1 };
	enemyPositions[5] = { 0, 6, 20, 1 };
	enemyPositions[6] = { -20, 4, 20, 1 };
	enemyPositions[7] = { -20, 2, 0, 1 };

	allyNum = 7;
	allyPositions = new vec4[allyNum];
	allyPositions[0] = { -20, 2, -20, 1 };
	allyPositions[1] = { 0, 2, -20, 2 };
	allyPositions[2] = { 20, 2, -20, 3 };
	allyPositions[3] = { 20, 4, 0, 1 };
	allyPositions[4] = { 20, 2, 20, 1 };
	allyPositions[5] = { 0, 2, 20, 2 };
	allyPositions[6] = { -20, 2, 20, 1 };
}
