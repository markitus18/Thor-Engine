#include "Globals.h"
#include "Application.h"
#include "ModuleScene5.h"
#include "Primitive.h"
#include "PhysBody3D.h"


ModuleScene5::ModuleScene5(Application* app, bool start_enabled) : ModuleScene(app, start_enabled)
{
}

void ModuleScene5::LoadPositions()
{
	neutralNum = 7;
	neutralPositions = new vec4[neutralNum];

	//Wall
	neutralPositions[0] = { -55, 1, 59, 4 };
	neutralPositions[1] = { -55, 1, 62, 4 };
	neutralPositions[2] = { -55, 1, 65, 4 };
	neutralPositions[3] = { -55, 1, 68, 4 };
	neutralPositions[4] = { -55, 1, 71, 4 };
	neutralPositions[5] = { -55, 1, 74, 4 };
	neutralPositions[6] = { -55, 1, 77, 4 };

	enemyNum = 3;
	enemyPositions = new vec4[enemyNum];
	#pragma region EnemyPositions

	enemyPositions[0] = { 2, 1, 43, 2 };
	enemyPositions[1] = { -58, 1, 67, 4 };
	enemyPositions[2] = { -9, 0, 67, 1 };
	

#pragma endregion EnemyPositions

	allyNum = 66;
	allyPositions = new vec4[allyNum];
#pragma region NeutralPositions
	//HALL
	//Behind Wall
	allyPositions[0] = { 1, 0, 1, 1 };
	allyPositions[1] = { 5, 0, 1, 1 };
	allyPositions[2] = { 9, 0, 1, 1 };
	allyPositions[3] = { -1,0, 1, 1 };
	allyPositions[4] = { -5, 0, 1, 1 };
	allyPositions[5] = { -9, 0, 1, 1 };

	//Left Wall
	allyPositions[6] = { 11, 0, 3, 1 };
	allyPositions[7] = { 11, 0, 7, 1 };
	allyPositions[8] = { 11, 0, 11, 1 };
	allyPositions[9] = { 11, 0, 15, 1 };
	allyPositions[10] = { 11, 0, 19, 1 };
	allyPositions[11] = { 11, 0, 23, 1 };

	//Right Wall
	allyPositions[12] = { -11, 0, 3, 1 };
	allyPositions[13] = { -11, 0, 7, 1 };
	allyPositions[14] = { -11, 0, 11, 1 };
	allyPositions[15] = { -11, 0, 15, 1 };
	allyPositions[16] = { -11, 0, 19, 1 };
	allyPositions[17] = { -11, 0, 23, 1 };

	//PASAGE
	//Pasage Start
	allyPositions[18] = { 7, 0, 23, 1 };
	allyPositions[19] = { -7, 0, 23, 1 };

	//Passage Left
	allyPositions[20] = { 7, 0, 27, 1 };
	allyPositions[21] = { 7, 0, 31, 1 };
	allyPositions[22] = { 7, 0, 35, 1 };
	allyPositions[23] = { 7, 0, 39, 1 };
	allyPositions[24] = { 7, 0, 43, 1 };
	allyPositions[25] = { 7, 0, 47, 1 };
	allyPositions[26] = { 7, 0, 51, 1 };
	allyPositions[27] = { 7, 0, 55, 1 };
	allyPositions[28] = { 7, 0, 59, 1 };
	allyPositions[29] = { 7, 0, 63, 1 };

	//Passage Right
	allyPositions[30] = { -7, 0, 25, 1 };
	allyPositions[31] = { -7, 0, 29, 1 };
	allyPositions[32] = { -7, 0, 33, 1 };
	allyPositions[33] = { -7, 0, 37, 1 };
	allyPositions[34] = { -7, 0, 41, 1 };
	allyPositions[35] = { -7, 0, 45, 1 };
	allyPositions[36] = { -7, 0, 49, 1 };
	allyPositions[37] = { -7, 0, 53, 1 };
	allyPositions[38] = { -7, 0, 57, 1 };
	allyPositions[39] = { -7, 0, 61, 1 };

	//End Start
	allyPositions[40] = { -7, 0, 63, 1 };
	allyPositions[41] = { -11, 0, 63, 1 };
	allyPositions[42] = { -15, 0, 63, 1 };

	allyPositions[43] = { 9, 0, 63, 1 };
	allyPositions[44] = { 13, 0, 63, 1 };

	//End Left
	allyPositions[45] = { 15, 0, 65, 1 };
	allyPositions[46] = { 15, 0, 69, 1 };


	//End End
	allyPositions[49] = { -13, 0, 71, 1 };
	allyPositions[50] = { -9, 0, 71, 1 };
	allyPositions[51] = { -5, 0, 71, 1 };
	allyPositions[52] = { -1, 0, 71, 1 };
	allyPositions[53] = { 3, 0, 71, 1 };
	allyPositions[47] = { 7, 0, 71, 1 };
	allyPositions[48] = { 11, 0, 71, 1 };

	// Right second pathway
	allyPositions[54] = { -19, 0, 63, 1 };
	allyPositions[55] = { -23, 0, 63, 1 };
	allyPositions[56] = { -27, 0, 63, 1 };
	allyPositions[57] = { -31, 0, 63, 1 };
	allyPositions[58] = { -35, 0, 63, 1 };
	allyPositions[59] = { -39, 0, 63, 1 };


	//Left second pathway
	allyPositions[62] = { -31, 0, 71, 1 };
	allyPositions[63] = { -27, 0, 71, 1 };
	allyPositions[64] = { -23, 0, 71, 1 };
	allyPositions[65] = { -19, 0, 71, 1 };
	allyPositions[61] = { -35, 0, 71, 1 };
	allyPositions[60] = { -39, 0, 71, 1 };


#pragma endregion NeutralPositions
	
}
