#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"

class GameObject;

class Component
{
public:
	enum Type
	{
		None,
		Transform, //?
		Mesh,
		Material,
		Camera,
	};

public:
	Component(Type type, const GameObject*);
	virtual ~Component();

	void SetActive(bool set);
	bool IsActive() const;
	Type GetType() const;

	//TMP: going to be public now to avoid some errors
	//Removed temporaly. Is it really necessary?
	const GameObject* gameObject;

protected:
	bool active;
	Type type;


};
#endif //__COMPONENT_H__
