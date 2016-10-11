#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"

class GameObject;

class Component
{
public:
	enum Type
	{
		Transform, //?
		Mesh,
		Material,
	};

public:
	Component(Type type, GameObject*);
	virtual ~Component();

	void SetActive(bool set);
	bool IsActive() const;
	Type GetType() const;

	//TMP: going to be public now to avoid some errors
	const GameObject* gameObject;

protected:
	bool active;
	Type type;


};
#endif //__COMPONENT_H__
