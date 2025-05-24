#include "EnemyRed.h"
#include "Sprite.h"
#include "TileMap.h"
#include "ResourceManager.h"
#include "Globals.h"
#include <algorithm>


EnemyRed::EnemyRed(const Point& startPos) : Entity(startPos,
        ENEMYRED_PHYSICAL_WIDTH, ENEMYRED_PHYSICAL_HEIGHT, ENEMYRED_FRAME_SIZE, ENEMYRED_FRAME_SIZE)
{
    e_direction = { 1, 0 };
    e_tileMap = nullptr;

    enemyX = (float)pos.x;
    enemyY = (float)pos.y;

}

EnemyRed::~EnemyRed()
{
}

AppStatus EnemyRed::Initialise() {

    enemyX = pos.x;
    enemyY = pos.y;

    const int n = ENEMYRED_FRAME_SIZE;

    auto& rm = ResourceManager::Instance();
    
    if (rm.LoadTexture(Resource::IMG_ENEMY_RED, "resources/Sprites/Enemies.png") != AppStatus::OK)
        return AppStatus::ERROR;

    render = new Sprite(rm.GetTexture(Resource::IMG_ENEMY_RED ));
    if (!render) {
        LOG("Failed to create enemy sprite");
        return AppStatus::ERROR;
    }

    Sprite* spr = dynamic_cast<Sprite*>(render);
    spr->SetNumberAnimations((int)EnemyRedAnim::NUM_ANIMATIONS);

    //right and up
    spr->SetAnimationDelay((int)EnemyRedAnim::WALK_RIGHT, ANIM_DELAY);
    for (int i = 0; i < 3; ++i)
        spr->AddKeyFrame((int)EnemyRedAnim::WALK_RIGHT, { (float)i * n, 0, n, n });

    //left and down
    spr->SetAnimationDelay((int)EnemyRedAnim::WALK_LEFT, ANIM_DELAY);
    for (int i = 0; i < 3; ++i)
        spr->AddKeyFrame((int)EnemyRedAnim::WALK_LEFT, { (float)(i + 3) * n, 0, n, n });

    spr->SetAnimation((int)EnemyRedAnim::WALK_RIGHT);
    return AppStatus::OK;
}

void EnemyRed::SetTileMap(TileMap* tilemap) {
    e_tileMap = tilemap;
}

void EnemyRed::Update() {

    UpdateAnimation();
    dynamic_cast<Sprite*>(render)->Update();

    if (!isMoving) {

        //CheckDirection();
        timer += GetFrameTime();

        pos.x = (pos.x / TILE_SIZE) * TILE_SIZE;
        pos.y = (pos.y / TILE_SIZE) * TILE_SIZE;
        enemyX = (float)pos.x;
        enemyY = (float)pos.y;


        LogicBrain();
        if (e_direction.x == 0 && e_direction.y == 0) {
            return;
        }
        targetTile = {
            pos.x + e_direction.x * TILE_SIZE,
            pos.y + e_direction.y * TILE_SIZE
        };
        isMoving = true;
    }
    if (isMoving) {
        enemyX += e_direction.x * ENEMYRED_SPEED;
        enemyY += e_direction.y * ENEMYRED_SPEED;

        bool reachedX = (e_direction.x > 0 && enemyX >= targetTile.x) ||
            (e_direction.x < 0 && enemyX <= targetTile.x);
        bool reachedY = (e_direction.y > 0 && enemyY >= targetTile.y) ||
            (e_direction.y < 0 && enemyY <= targetTile.y);

        if ((e_direction.x != 0 && reachedX) || (e_direction.y != 0 && reachedY)) {
            enemyX = (float)targetTile.x;
            enemyY = (float)targetTile.y;
            pos = targetTile;
            isMoving = false;
        }
        else {
            pos = { (int)enemyX, (int)enemyY };
        }
    }
}

void EnemyRed::LogicBrain() {
    printf("Checking walkability at (%d, %d)\n", pos.x, pos.y);

    std::vector<Point> possibleDirection;
    std::random_device rd;
    std::mt19937 gen(rd());

    int Px = pos.x / TILE_SIZE;
    int Py = pos.y / TILE_SIZE - 1;


    if (IsTileWalkable(Px, Py - 1)) possibleDirection.push_back({ 0, -1 });
    if (IsTileWalkable(Px, Py + 1)) possibleDirection.push_back({ 0, 1 });
    if (IsTileWalkable(Px - 1, Py)) possibleDirection.push_back({ -1, 0 });
    if (IsTileWalkable(Px + 1, Py)) possibleDirection.push_back({ 1, 0 });
    Point reverseMov = { -lastDirection.x, -lastDirection.y };
    possibleDirection.erase(
        std::remove(possibleDirection.begin(), possibleDirection.end(), reverseMov), possibleDirection.end()
    );

    if (!possibleDirection.empty()) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, (int)possibleDirection.size() - 1);
        e_direction = possibleDirection[dis(gen)];
    }

    else if (IsTileWalkable(Px + reverseMov.x, Py + reverseMov.y)) {
        e_direction = reverseMov;
    }
    else {
        e_direction = { 0, 0 };
    }
    timer = 0;
    lastDirection = e_direction;
}

bool EnemyRed::IsTileWalkable(int x, int y) {
    if (x < 0 || x >= e_tileMap->width || y < 0 || y >= e_tileMap->height)
        return false;

    Tile tile = e_tileMap->GetTileIndex(x, y);
    return !e_tileMap->IsTileSolid(tile);
}

void EnemyRed::CheckDirection() {
    AABB box = GetHitbox();
    int candidate = enemyY + e_direction.y * ENEMYRED_SPEED;
    if (e_tileMap->TestCollisionGround(box, &candidate)) {
        canMoveDown = false;
    }
    else
    {
        canMoveDown = true;
    }

    if (e_tileMap->TestCollisionWallUp(box)) {
        canMoveUp = false;
    }
    else
    {
        canMoveUp = true;
    }
    if (e_tileMap->TestCollisionWallLeft(box)) {
        canMoveLeft = false;
    }
    else
    {
        canMoveLeft = true;
    }
    if (e_tileMap->TestCollisionWallRight(box)) {
        canMoveRight = false;
    }
    else
    {
        canMoveRight = true;
    }
}

void EnemyRed::MoveX() {
    if (!e_tileMap) return;

    float nextX = enemyX + e_direction.x * ENEMYRED_SPEED;
    AABB futurePos = GetHitbox();
    futurePos.pos.x = (int)nextX;

    if ((e_direction.x < 0 && e_tileMap->TestCollisionWallLeft(futurePos)) ||
        (e_direction.x > 0 && e_tileMap->TestCollisionWallRight(futurePos))) {
        return;
    }

    enemyX = nextX;
}

void EnemyRed::MoveY() {
    if (!e_tileMap) return;
    int prevY = pos.y;

    AABB box = GetHitbox();

    if (e_direction.y < 0) {
        if (e_tileMap->TestCollisionWallUp(box)) {
            int tileY = box.pos.y / TILE_SIZE;
            pos.y = (tileY + 1) * TILE_SIZE + (box.height - 1);
        }
    }
    else if (e_direction.y > 0) {
        int candidateY = pos.y;
        if (e_tileMap->TestCollisionGround(box, &candidateY)) {
            pos.y = candidateY;
        }
    }
}

void EnemyRed::UpdateAnimation() {
    Sprite* spr = dynamic_cast<Sprite*>(render);
    if (e_direction.y < 0 || e_direction.x > 0)
        spr->SetAnimation((int)EnemyRedAnim::WALK_RIGHT);
    else
        spr->SetAnimation((int)EnemyRedAnim::WALK_LEFT);
}

void EnemyRed::Draw() const {
    dynamic_cast<Sprite*>(render)->Draw((int)pos.x - 1, (int)pos.y - height);
    Entity::DrawHitbox(pos.x, pos.y, width, height, WHITE);
}

void EnemyRed::Release() {
    ResourceManager::Instance().ReleaseTexture(Resource::IMG_ENEMY_RED);
    render->Release();
}
