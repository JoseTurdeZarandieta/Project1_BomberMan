#include "Object.h"
#include "StaticImage.h"

Object::Object(const Point& p, ObjectType t) : Entity(p, OBJECT_PHYSICAL_SIZE, OBJECT_PHYSICAL_SIZE, OBJECT_FRAME_SIZE, OBJECT_FRAME_SIZE)
{
	type = t; 
	
	Rectangle rc;
	const int n = TILE_SIZE;
	switch (type)
	{
		case ObjectType::FIRE_UP: rc = {4*n, 3*n, n, n}; break;
		case ObjectType::BOMB_UP: rc = {5*n, 3*n, n, n}; break;

		default: LOG("Internal error: object creation of invalid type");
	}

	ResourceManager& data = ResourceManager::Instance();
	render = new StaticImage(data.GetTexture(Resource::IMG_TILES), rc);
}
Object::~Object()
{
}
void Object::DrawDebug(const Color& col) const
{
	Entity::DrawHitbox(pos.x, pos.y, width, height, col);
}
int Object::Points() const
{
	if (type == ObjectType::BOMB_UP)		return POINTS_BOMB_UP;
	else if (type == ObjectType::FIRE_UP)	return POINTS_FIRE_UP;
	else
	{
		LOG("Internal error: object type invalid when giving points");
		return 0;
	}
}