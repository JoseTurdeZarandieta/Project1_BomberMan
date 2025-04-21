#include "Enemy.h"
#include "Sprite.h"
#include "TileMap.h"
#include "ResourceManager.h"
#include "Globals.h"
#include <algorithm>


Enemy::Enemy(const Point& startPos) : Entity(startPos,
        ENEMY_PHYSICAL_WIDTH, ENEMY_PHYSICAL_HEIGHT, ENEMY_FRAME_SIZE, ENEMY_FRAME_SIZE)
{
    e_direction = { 1, 0 };
    e_tileMap = nullptr;
    
}

Enemy::~Enemy()
{
}

AppStatus Enemy::Initialise() {

    enemyX = pos.x;
    enemyY = pos.y;

    const int n = ENEMY_FRAME_SIZE;

    auto& rm = ResourceManager::Instance();
    
    if (rm.LoadTexture(Resource::IMG_ENEMY, "resources/Sprites/Enemies.png") != AppStatus::OK)
        return AppStatus::ERROR;

    render = new Sprite(rm.GetTexture(Resource::IMG_ENEMY));
    if (!render) {
        LOG("Failed to create enemy sprite");
        return AppStatus::ERROR;
    }

    Sprite* spr = dynamic_cast<Sprite*>(render);
    spr->SetNumberAnimations((int)EnemyAnim::NUM_ANIMATIONS);

    //right and up
    spr->SetAnimationDelay((int)EnemyAnim::WALK_RIGHT, ANIM_DELAY);
    for (int i = 0; i < 3; ++i)
        spr->AddKeyFrame((int)EnemyAnim::WALK_RIGHT, { (float)i * n, 0, n, n });

    //left and down
    spr->SetAnimationDelay((int)EnemyAnim::WALK_LEFT, ANIM_DELAY);
    for (int i = 0; i < 3; ++i)
        spr->AddKeyFrame((int)EnemyAnim::WALK_LEFT, { (float)(i + 3) * n, 0, n, n });

    spr->SetAnimation((int)EnemyAnim::WALK_RIGHT);
    return AppStatus::OK;
}

void Enemy::SetTileMap(TileMap* tilemap) {
    e_tileMap = tilemap;
}

void Enemy::Update() {

    CheckDirection();
    timer += GetFrameTime();
    LogicBrain();
    //MoveY();
    //MoveX();
    UpdateAnimation();

    enemyX += e_direction.x * ENEMY_SPEED; //first movement of enemy
    enemyY += e_direction.y * ENEMY_SPEED;
    pos = { (int)enemyX, (int)enemyY };

    dynamic_cast<Sprite*>(render)->Update();
}

void Enemy::LogicBrain() {

    std::vector<Point> possibleDirection;
    std::random_device rd;
    std::mt19937 gen(rd());
    

    if (canMoveUp) {
        possibleDirection.emplace_back(Point{ 0,-1 });
    }
    if (canMoveDown) {
        possibleDirection.emplace_back(Point{ 0,1 });
    }
    if (canMoveRight) {
        possibleDirection.emplace_back(Point{ 1,0 });
    }
    if (canMoveLeft) {
        possibleDirection.emplace_back(Point{ -1,0 });
    }
    printf("%f\n", timer);

    if (possibleDirection.size() == 0) {
        selectedDirection = { 0,0 };
    }
    else if (std::find(possibleDirection.begin(), possibleDirection.end(), selectedDirection) != possibleDirection.end()) {
        if (timer > 2) {
            std::uniform_int_distribution<> distr(0, possibleDirection.size()-1);
            int randomIndex = distr(gen);
            selectedDirection = possibleDirection[randomIndex];
            timer = 0;
        }
    }
    else {
        std::uniform_int_distribution<> distr(0, possibleDirection.size()-1);
        int randomIndex = distr(gen);
        selectedDirection = possibleDirection[randomIndex];
        //timer = 0;
    }
    e_direction = selectedDirection;
}

void Enemy::MoveX() {
    if (!e_tileMap) return;
    int prevX = pos.x;

    AABB box = GetHitbox();

    if (e_direction.x < 0) {
        if (e_tileMap->TestCollisionWallLeft(box)) {
            int tileX = box.pos.x / TILE_SIZE;
            pos.x = (tileX + 1) * TILE_SIZE;
        }
    }
    else if (e_direction.x > 0) {
        if (e_tileMap->TestCollisionWallRight(box)) {
            int tileX = (box.pos.x + box.width - 1) / TILE_SIZE;
            pos.x = tileX * TILE_SIZE - box.width;
        }
    }
}

void Enemy::MoveY() {
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

void Enemy::CheckDirection() {
    AABB box = GetHitbox();
    int candidate = pos.y;
    if (e_tileMap->TestCollisionGround(box, &candidate)) {
        canMoveDown = false;
    }
    else {
        canMoveDown = true;
    }

    if (e_tileMap->TestCollisionWallUp(box)) {
        canMoveUp = false;
    }
    else {
        canMoveUp = true;
    }

    if (e_tileMap->TestCollisionWallLeft(box)) {
        canMoveLeft = false;
    }
    else {
        canMoveLeft = true;
    }

    if (e_tileMap->TestCollisionWallRight(box)) {
        canMoveRight = false;
    }
    else {
        canMoveRight = true;
    }
}



void Enemy::UpdateAnimation() {
    Sprite* spr = dynamic_cast<Sprite*>(render);
    //up or right
    if (e_direction.y < 0 || e_direction.x > 0)
        spr->SetAnimation((int)EnemyAnim::WALK_RIGHT);
    //down or left
    else
        spr->SetAnimation((int)EnemyAnim::WALK_LEFT);
}

void Enemy::Draw() const {
    dynamic_cast<Sprite*>(render)->Draw((int)pos.x, (int)pos.y);
    Entity::DrawHitbox(pos.x, pos.y, width, height, WHITE);
}

void Enemy::Release() {
    ResourceManager::Instance().ReleaseTexture(Resource::IMG_ENEMY);
    render->Release();
}
