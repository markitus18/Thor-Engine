#ifndef __SELECTABLE_H__
#define __SELECTABLE_H__

enum Selectable_Type
{
	GAMEOBJECT,
	RESOURCE_TEXTURE,
};

template <class Type>
class Selectable
{
public:
	Selectable(Type* data, Selectable_Type type)
	{
		this->data = data;
		this->type = type;
	}

	Type* GetData() const { return data; }
	Selectable_Type GetType() const { return type; }

private:
	Type* data;
	Selectable_Type type;
};

#endif // !__SELECTABLE_H__
