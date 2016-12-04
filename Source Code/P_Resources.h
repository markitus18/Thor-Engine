#ifndef __P_RESOURCES_H__
#define __P_RESOURCES_H__

#include "Panel.h"

class Resource;

class P_Resources : public Panel
{
public:
	P_Resources();
	~P_Resources();

	void Draw(ImGuiWindowFlags flags);
	void DisplayResourceInfo(Resource* resource);

	void UpdatePosition(int, int);

};

#endif //__P_RESOURCES_H__

