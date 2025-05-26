#pragma once
#include <raylib.h>
#include "Sprite.h"
#include "Point.h"
#include "AABB.h"
#include "Globals.h"
#include <unordered_map>
#include "EnemyRed.h"
#include "EnemyBlue.h"
#include <vector>

enum class Tile {

	// -1: area covered by an entity
	EMPTY = -1,
	//  0: air tile
	AIR = 0,

	// 0 < id < 50: static tiles
	GREEN_BLOCK = 1, BLOCK = 2, SOFT_BLOCK, BOMB,
	
	

	// 50 <= id < 100: special tiles
	// 60s items -> 70s secret items 
	DOOR = 50,
	ITEM_BOMB_UP = 60, ITEM_FIRE_UP, ITEM_SPEED_UP, ITEM_REMOTE_CONTROL, ITEM_WALL_PASS, ITEM_BOMB_PASS, ITEM_FLAME_PASS, ITEM_INVINCIBLE,
	ITEM_B_PANEL = 70, ITEM_GODDESS, ITEM_COLA, ITEM_FAMICOM, ITEM_PROGRAMMER_NAKAMOTO, ITEM_DEZENIMAN,

	// id >= 100: entities' initial locations
	PLAYER = 100,ENEMY_RED = 101, ENEMY_BLUE = 102,

	//Intervals hay que mirarlo

	
};

class TileMap
{
public:
	TileMap();
	~TileMap();

	AppStatus Initialise();
	AppStatus Load(int data[], int w, int h);
	void Update();
	void Render();
	void Release();

	Tile GetObjectAtPosition(const AABB& box, int* px) const;

	bool IsTileDoor(Tile tile) const;

	//Test for collisions with walls
	bool TestCollisionWallLeft(const AABB& box) const;
	bool TestCollisionWallRight(const AABB& box) const;
	bool TestCollisionGround(const AABB& box, int* py) const;
	bool TestCollisionWallUp(const AABB& box) const;
	bool TestCollisionWallDown(const AABB& box) const;
	
	
	

public:
	void InitTileDictionary();

	Tile GetTileIndex(int x, int y) const;
	bool IsTileSolid(Tile tile) const;
	bool CollisionX(const Point& p, int distance) const;
	bool CollisionY(const Point& p, int distance) const;

	//Tile map
	Tile *map;

	//Size of the tile map
	int size, width, height;
	
	//Dictionary of tile frames
	std::unordered_map<int, Rectangle> dict_rect;

	//Tile sheet
	const Texture2D *img_tiles;
	const Texture2D *img_Items;
};