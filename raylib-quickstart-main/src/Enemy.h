#pragma once
#include "Entity.h"
#include "TileMap.h"
#include "Globals.h"
#include <raylib.h>
#include <random>


#define ENEMY_FRAME_SIZE      16
#define ENEMY_PHYSICAL_WIDTH  8
#define ENEMY_PHYSICAL_HEIGHT 8
#define ENEMY_SPEED           0.5

enum class EnemyAnim {
    WALK_RIGHT = 0,
    WALK_LEFT = 1,
    NUM_ANIMATIONS
};

class TileMap;

class Enemy : public Entity {
public:
    Enemy(const Point& startPos);
    ~Enemy();

    AppStatus Initialise();

    void SetTileMap(TileMap* tilemap);

    void Update();
    void Draw() const;
    void Release();

    void SetDirection(int dx, int dy);

private:
    void MoveX();
    void MoveY();

    void UpdateAnimation();

    void CheckDirection();

    bool canMoveLeft    = false;
    bool canMoveRight   = true;
    bool canMoveUp      = false;
    bool canMoveDown    = false;

    float enemyX;
    float enemyY;

    float timer = 2;
    Point selectedDirection = { 1,0 };
    void LogicBrain();

    Point       e_direction;
    TileMap*    e_tileMap;
};
