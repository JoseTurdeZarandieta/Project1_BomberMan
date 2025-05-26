#include "Explosion.h"



Explosion::Explosion(const Point& p, explosionAnim type) : pos(p), animType(type){}

Explosion::~Explosion() {
    if (spr) {
        spr->Release();
        delete spr;
    }
    ResourceManager::Instance().ReleaseTexture(Resource::IMG_EXPLOSION);
}

AppStatus Explosion::Initialise() {

    auto& rm = ResourceManager::Instance();
    if (rm.LoadTexture(Resource::IMG_EXPLOSION, "resources/Sprites/General.png") != AppStatus::OK) {
        LOG("Failed to load explosion texture");
        return AppStatus::ERROR;
    }
    spr = new Sprite(rm.GetTexture(Resource::IMG_EXPLOSION));
    spr->SetNumberAnimations((int)explosionAnim::NUM_ANIMATIONS);

    const int n = EXPLOSION_TILE_SIZE;

    //left
    spr->SetAnimationDelay((int)explosionAnim::EXPLOSION_LEFT, ANIM_DELAY);
    for (int i = 0; i < 4; ++i)
        spr->AddKeyFrame((int)explosionAnim::EXPLOSION_LEFT, { (float)i * n, 4 * n, n, n });

    //right
    spr->SetAnimationDelay((int)explosionAnim::EXPLOSION_RIGHT, ANIM_DELAY);
    for (int i = 0; i < 4; ++i)
        spr->AddKeyFrame((int)explosionAnim::EXPLOSION_RIGHT, { (float)(i + 4) * n, 4 * n, n, n });

    //down
    spr->SetAnimationDelay((int)explosionAnim::EXPLOSION_DOWN, ANIM_DELAY);
    for (int i = 0; i < 4; ++i)
        spr->AddKeyFrame((int)explosionAnim::EXPLOSION_DOWN, { (float)i * n, 5 * n, n, n });

    //up
    spr->SetAnimationDelay((int)explosionAnim::EXPLOSION_UP, ANIM_DELAY);
    for (int i = 0; i < 4; ++i)
        spr->AddKeyFrame((int)explosionAnim::EXPLOSION_UP, { (float)(i + 4) * n, 5 * n, n, n });

    //center
    spr->SetAnimationDelay((int)explosionAnim::EXPLOSION_CENTER, ANIM_DELAY);
    for (int i = 0; i < 4; ++i)
        spr->AddKeyFrame((int)explosionAnim::EXPLOSION_CENTER, { (float)i * n, 6 * n, n, n });

    //mid-hor
    spr->SetAnimationDelay((int)explosionAnim::EXPLOSION_MID_HOR, ANIM_DELAY);
    for (int i = 0; i < 4; ++i)
        spr->AddKeyFrame((int)explosionAnim::EXPLOSION_MID_HOR, { (float)(i + 4) * n, 6 * n, n, n });

    //mid-ver
    spr->SetAnimationDelay((int)explosionAnim::EXPLOSION_MID_VER, ANIM_DELAY);
    for (int i = 0; i < 4; ++i)
        spr->AddKeyFrame((int)explosionAnim::EXPLOSION_MID_VER, { (float)i * n, 7 * n, n, n });

    spr->SetAnimation((int)animType);
    return AppStatus::OK;
}

void Explosion::Update(float dt) {
    timer += dt;
    spr->Update();
}

void Explosion::Draw() const {
    spr->Draw(pos.x, pos.y);
}

bool Explosion::IsFinished() const{
    return timer >= duration;
}