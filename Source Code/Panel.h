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
	p2Point<int> position;
	p2Point<int> size;

public:
	bool active = true;
};

#endif //__PANEL_H__
