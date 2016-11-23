#ifndef __M_RESOURCES_H__
#define __M_RESOURCES_H__

#include "Module.h"

class M_Resources : public Module
{
public:
	M_Resources(Application* app, bool start_enabled = true);
	~M_Resources();

	bool Init(Config& config);
	bool CleanUp();
};

#endif
