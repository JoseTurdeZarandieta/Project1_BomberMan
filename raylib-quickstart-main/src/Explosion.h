#pragma once
#include "Point.h"
#include "Sprite.h"
#include "RenderComponent.h"
#include "Globals.h"
#include"TileMap.h"
#include "Globals.h"
#include "ResourceManager.h"
#include <algorithm>


#define EXPLOSION_TILE_SIZE	16

enum class explosionAnim {
	EXPLOSION_LEFT = 0,
	EXPLOSION_RIGHT,
	EXPLOSION_DOWN,
	EXPLOSION_UP,
	EXPLOSION_CENTER,
	EXPLOSION_MID_HOR,
	EXPLOSION_MID_VER,
	NUM_ANIMATIONS
};

class Explosion {
public:
	Explosion(const Point&, explosionAnim type);
	~Explosion();

	AppStatus Initialise();

	void Update(float dt);
	void Draw() const;
	bool IsFinished() const;


private:
	Point pos;
	explosionAnim animType;
	Sprite* spr = nullptr;
	float timer = 0.0f;
	const float duration = 1.0f;
};