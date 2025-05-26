#pragma once
#include "Entity.h"
#include "TileMap.h"
#include "Globals.h"
#include <raylib.h>
#include <random>


#define ENEMYRED_FRAME_SIZE      16
#define ENEMYRED_PHYSICAL_WIDTH  16
#define ENEMYRED_PHYSICAL_HEIGHT 16
#define ENEMYRED_SPEED           1.0f

enum class EnemyRedAnim {
    WALK_RIGHT = 0,
    WALK_LEFT = 1,
    NUM_ANIMATIONS
};

class TileMap;

class EnemyRed : public Entity {
public:
    EnemyRed(const Point& startPos);
    ~EnemyRed();

    AppStatus Initialise();

    void SetTileMap(TileMap* tilemap);

    void Update();
    void Draw() const;
    void Release();

    void UpdateAnimation();

private:
    void MoveX();
    void MoveY();

    void CheckDirection();

    bool canMoveLeft    = false;
    bool canMoveRight   = false;
    bool canMoveUp      = false;
    bool canMoveDown    = false;

    float enemyX;
    float enemyY;

    float timer = 4;
    Point selectedDirection = { 1,0 };
    void LogicBrain();

    Point lastDirection = { 0, 0 };
    bool IsTileWalkable(int x, int y);
    Point targetTile;
    bool isMoving = false;

    Point       e_direction;
    TileMap*    e_tileMap;
};
