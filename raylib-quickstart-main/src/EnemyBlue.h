#pragma once
#include "Entity.h"
#include "TileMap.h"
#include "Globals.h"
#include <raylib.h>
#include <random>


#define ENEMYBLUE_FRAME_SIZE      16
#define ENEMYBLUE_PHYSICAL_WIDTH  16
#define ENEMYBLUE_PHYSICAL_HEIGHT 16
#define ENEMYBLUE_SPEED           1.0f

enum class EnemyBlueAnim {
    WALK_RIGHT = 0,
    WALK_LEFT = 1,
    NUM_ANIMATIONS
};

class TileMap;

class EnemyBlue : public Entity {
public:
    EnemyBlue(const Point& startPos);
    ~EnemyBlue();

    AppStatus Initialise();

    void SetTileMap(TileMap* tilemap);

    void UpdateBlue();
    void DrawBlue() const;
    void ReleaseBlue();


private:
    void MoveX();
    void MoveY();

    void UpdateAnimation();

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
