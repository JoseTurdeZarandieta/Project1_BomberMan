#include "AudioManager.h"
#include "Scene.h"
#include <stdio.h>
#include "Globals.h"
#include "Entity.h"
#include "Enemy.h"
#include "player.h"

Scene::Scene()
{
	player = nullptr;
    level = nullptr;
	
	camera.target = { 0, 0 };				//Center of the screen
	camera.offset = { 0, MARGIN_GUI_Y };	//Offset from the target (center of the screen)
	camera.rotation = 0.0f;					//No rotation
	camera.zoom = 1.0f;						//Default zoom

	debug = DebugMode::OFF;
}
Scene::~Scene()
{
	if (player != nullptr)
	{
		player->Release();
		delete player;
		player = nullptr;
	}
    if (level != nullptr)
    {
		level->Release();
        delete level;
        level = nullptr;
    }
	for (Entity* obj : objects)
	{
		delete obj;
	}
	objects.clear();
	
	for (Entity* enem : enemies)
	{
		delete enem;
	}
	enemies.clear();
}
AppStatus Scene::Init()
{
	//Create player
	player = new Player({ 16,32 }, State::IDLE, Look::RIGHT);
	if (player == nullptr)
	{
		LOG("Failed to allocate memory for Player");
		return AppStatus::ERROR;
	}
	//Initialise player
	if (player->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Player");
		return AppStatus::ERROR;
	}

	//Create level 
    level = new TileMap();
    if (level == nullptr)
    {
        LOG("Failed to allocate memory for Level");
        return AppStatus::ERROR;
    }
	//Initialise level
	if (level->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Level");
		return AppStatus::ERROR;
	}
	//Load level
	if (LoadLevel(1) != AppStatus::OK)
	{
		LOG("Failed to load Level 1");
		return AppStatus::ERROR;
	}
	//Assign the tile map reference to the player to check collisions while navigating
	player->SetTileMap(level);

	//create and initialise enemy
	Enemy* enemy = new Enemy({ 160, 184 });
	enemy->SetTileMap(level);
	if (enemy->Initialise() != AppStatus::OK) {
		return AppStatus::ERROR;
	}
	enemies.push_back(enemy);

    return AppStatus::OK;
}
AppStatus Scene::LoadLevel(int stage)
{
	int size;
	int x, y, i;
	Tile tile;
	Point pos;
	int *map = nullptr;
	Object *obj;
	
	ClearLevel();

	size = LEVEL_WIDTH * LEVEL_HEIGHT;
	if (stage == 1)
	{
		currentstage = 1;
		map = new int[size] {
			 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,	  2,   2,   2,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   3,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   3,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   3,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   3,   0,   3,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2
		};
		player->InitScore();
	}
	else if (stage == 2)
	{
		currentstage = 2;
		map = new int[size] {
			 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,	  2,   2,   2,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2
		};
	}
	else if (stage == 3)
	{
	currentstage = 3;
		map = new int[size] {
			 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,	  2,   2,   2,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   3,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   3,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   3,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   3,   0,   3,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2
		};
	}
	else if (stage == 4)
	{
	currentstage = 4;
		map = new int[size] {
			  2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,	  2,   2,   2,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2
		};
	}
	else if (stage == 5)
	{
	currentstage = 5;
		map = new int[size] {
			 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,	  2,   2,   2,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   3,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2
		};
	}
	else if (stage == 6)
	{
	currentstage = 6;
		map = new int[size] {
			 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,	  2,   2,   2,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   3,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   3,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2
		};
	}
	else if (stage == 7)
	{
	currentstage = 7;
		map = new int[size] {
			 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,	  2,   2,   2,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   0,
			 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2
		};
	}
	else if (stage == 8)
	{
	currentstage = 8;
		map = new int[size] {
			 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,	  2,   2,   2,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,   0,   2,	  0,   2,   0,   2,
			 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   2,
			 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2
		};
	}
	else
	{
		//Error level doesn't exist or incorrect level number
		LOG("Failed to load level, stage %d doesn't exist", stage);
		return AppStatus::ERROR;	
	}

	//Entities and objects
	i = 0;
	for (y = 0; y < LEVEL_HEIGHT; ++y)
	{
		for (x = 0; x < LEVEL_WIDTH; ++x)
		{
			tile = (Tile)map[i];
			if (tile == Tile::EMPTY)
			{
				map[i] = 0;
			}
			else if (tile == Tile::PLAYER)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				player->SetPos(pos);
				map[i] = 0;
			}
			else if (tile == Tile::ITEM_BOMB_UP)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				obj = new Object(pos, ObjectType::BOMB_UP);
				objects.push_back(obj);
				map[i] = 0;
			}
			else if (tile == Tile::ITEM_FIRE_UP)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				obj = new Object(pos, ObjectType::FIRE_UP);
				objects.push_back(obj);
				map[i] = 0;
			}
			++i;
		}
	}
	//Tile map
	level->Load(map, LEVEL_WIDTH, LEVEL_HEIGHT);

	return AppStatus::OK;
}
void Scene::Update()
{
	Point p1, p2;
	AABB box;

	//Switch between the different debug modes: off, on (sprites & hitboxes), on (hitboxes) 
	if (IsKeyPressed(KEY_F1))
	{
		debug = (DebugMode)(((int)debug + 1) % (int)DebugMode::SIZE);
	}
	else if (IsKeyPressed(KEY_F2))
	{
		victory = true;
	}
	else if (IsKeyPressed(KEY_F3))
	{
		game_over = true;
	}
	if (IsKeyPressed(KEY_F4))
	{
		player->health -= 1;
	}
	if (player->GetHealth() <= 0)
	{
		game_over = true;

	}
	//Debug levels instantly
	if (IsKeyPressed(KEY_ONE))		LoadLevel(1);
	else if (IsKeyPressed(KEY_TWO))	LoadLevel(2);
	else if (IsKeyPressed(KEY_THREE))	LoadLevel(3);
	else if (IsKeyPressed(KEY_FOUR))	LoadLevel(4);
	else if (IsKeyPressed(KEY_FIVE))	LoadLevel(5);
	else if (IsKeyPressed(KEY_SIX))	LoadLevel(6);
	else if (player->NextLevel() && currentstage == 1)
	{
		LoadLevel(2);

		player->SetPos({ 20, player->GetY() });
	}
	else if (player->PrevLevel() && currentstage == 2)
	{
		LoadLevel(1);

		player->SetPos({ 390, player->GetY() });
	}
	else if (player->NextLevel() && currentstage == 3)
	{
		LoadLevel(4);

		player->SetPos({ 20, player->GetY() });
	}
	else if (player->PrevLevel() && currentstage == 4)
	{
		LoadLevel(3);

		player->SetPos({ 390, player->GetY() });
	}
	else if (player->NextLevel() && currentstage == 5)
	{
		LoadLevel(6);

		player->SetPos({ 20, player->GetY() });
	}
	else if (player->PrevLevel() && currentstage == 6)
	{
		LoadLevel(5);

		player->SetPos({ 390, player->GetY() });
	}
	else if (player->NextLevel() && currentstage == 7)
	{
		LoadLevel(8);

		player->SetPos({ 20, player->GetY() });
	}
	else if (player->PrevLevel() && currentstage == 8)
	{
		LoadLevel(7);

		player->SetPos({ 390, player->GetY() });
	}
	if (IsKeyPressed(KEY_D))
	{
		// Iniciar el temporizador y almacenar la posición actual del jugador
		AudioManager::Instance().PlaySoundByName("BombDown");

		Point bombPos = player->GetPos();
		int tileX = bombPos.x / TILE_SIZE;
		int tileY = bombPos.y / TILE_SIZE;




		// Cambiar la tile en la posición del jugador

		level->map[tileY * LEVEL_WIDTH + tileX] = Tile::BOMB;

		Player::Bomb newBomb = { bombPos, 0.0f };
		player->activeBombs.push_back(newBomb);
	}

	for (int i = 0; i < player->activeBombs.size();)
	{
		player->activeBombs[i].timer += GetFrameTime();

		if (player->activeBombs[i].timer > 3.0f) {
			AudioManager::Instance().PlaySoundByName("BombExplode");
			// Calcular la posición de las tiles adyacentes al jugador
			int tileX = player->activeBombs[i].position.x / TILE_SIZE; // Asumiendo que TILE_SIZE es el tamaño de una tile
			int tileY = player->activeBombs[i].position.y / TILE_SIZE;
			// Modificar el valor de las tiles en todas las direcciones a 0, incluyendo la posición del jugador
			for (int dx = -1; dx <= 1; ++dx)
			{
				for (int dy = -1; dy <= 1; ++dy)
				{
					int adjTileX = tileX + dx;
					int adjTileY = tileY + dy;
					int tileIndex = adjTileY * LEVEL_WIDTH + adjTileX;


					if (level->map[tileIndex] != Tile::BLOCK)
					{
						if (adjTileX == 5 && adjTileY == 5)
						{
							level->map[tileIndex] = Tile::DOOR;
						}
						else {

							level->map[tileIndex] = Tile::AIR;
						}// Asumiendo que Tile::AIR es el valor para una tile vacía
					}
					//else if (adjTileX == enemy)
					if (adjTileX == player->GetX() / TILE_SIZE && adjTileY == player->GetY() / TILE_SIZE)
					{
						player->takeDamage(1);
						if (player->GetHealth() <= 0)
						{
							game_over = true;

						}
					}
				}
			}
			player->activeBombs.erase(player->activeBombs.begin() + i);




		}
		else
		{
			++i;
		}
	}
	if (player->victory)
	{
		victory = true;
	}

	level->Update();
	player->Update();
	for (Enemy* enemy : enemies)
		enemy->Update();
	CheckCollisions();

	camera.target = {
		static_cast<float>(player->GetX()), static_cast<float>(player->GetY()) - WINDOW_HEIGHT / 2 + MARGIN_GUI_Y
	};


}
void Scene::Render()
{
	BeginMode2D(camera);

    level->Render();
	if (debug == DebugMode::OFF || debug == DebugMode::SPRITES_AND_HITBOXES)
	{
		RenderObjects(); 
		player->Draw();
		for (Enemy* enemy : enemies)
			enemy->Draw();
	}
	if (debug == DebugMode::SPRITES_AND_HITBOXES || debug == DebugMode::ONLY_HITBOXES)
	{
		RenderObjectsDebug(YELLOW);
		player->DrawDebug(GREEN);
	}

	EndMode2D();

	RenderGUI();
}
void Scene::Release()
{
    level->Release();
	player->Release();
	ClearLevel();
}
void Scene::CheckCollisions()
{
	AABB player_box, obj_box;
	
	player_box = player->GetHitbox();
	auto it = objects.begin();
	while (it != objects.end())
	{
		obj_box = (*it)->GetHitbox();
		if(player_box.TestAABB(obj_box))
		{
			player->IncrScore((*it)->Points());
			
			//Delete the object
			delete* it; 
			//Erase the object from the vector and get the iterator to the next valid element
			it = objects.erase(it); 
		}
		else
		{
			//Move to the next object
			++it; 
		}
	}
}
void Scene::ClearLevel()
{
	for (Object* obj : objects)
	{
		delete obj;
	}
	objects.clear();
	for (Enemy* enem : enemies)
	{
		delete enem;
	}
	enemies.clear();
}
void Scene::RenderObjects() const
{
	for (Object* obj : objects)
	{
		obj->Draw();
	}
}
void Scene::RenderObjectsDebug(const Color& col) const
{
	for (Object* obj : objects)
	{
		obj->DrawDebug(col);
	}
}
void Scene::RenderGUI() const
{
	//Temporal approach
	DrawText(TextFormat("SCORE : %d", player->GetScore()), 10, 10, 8, LIGHTGRAY);
	DrawText(TextFormat("HEALTH : %d", player->GetHealth()), 325, 10, 8, LIGHTGRAY);
}