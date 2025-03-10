#pragma once
#include "Entity.h"

#define OBJECT_PHYSICAL_SIZE	14
#define OBJECT_FRAME_SIZE		16

#define POINTS_BOMB_UP	10
#define POINTS_FIRE_UP	20

enum class ObjectType { BOMB_UP, FIRE_UP };

class Object : public Entity
{
public:
	Object(const Point& p, ObjectType t);
	~Object();

	void DrawDebug(const Color& col) const;
	int Points() const;

private:
	ObjectType type;
};

