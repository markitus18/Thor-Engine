#ifndef __PANEL_H__
#define __PANEL_H__

#include "p2Point.h"

class Panel
{
public:
	Panel();
	virtual ~Panel();

	virtual void Draw();

protected:
	bool active = true;
	p2Point<int> position;
	p2Point<int> size;
};

#endif //__PANEL_H__
