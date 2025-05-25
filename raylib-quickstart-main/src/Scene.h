#pragma once
#include <raylib.h>
#include "Player.h"
#include "TileMap.h"
#include "Object.h"
#include "EnemyRed.h"
#include "EnemyBlue.h"
#include "Explosion.h"

enum class DebugMode { OFF, SPRITES_AND_HITBOXES, ONLY_HITBOXES, SIZE };

class Scene
{
public:
    Scene();
    ~Scene();

    AppStatus Init();
    bool LoadMapFromFile(const std::string& filename, int* map, int size);
    void Update();
    void Render();
    void Release();
    bool startTimer;
    float timer;
    Point initialPlayerPos;

public:
    AppStatus LoadLevel(int stage);
    
    void CheckCollisions(); 
    void ClearLevel();
    void RenderObjects() const;
    void RenderObjectsDebug(const Color& col) const;

    void RenderGUI() const;
   
    int currentstage = 1;
    Player *player;
    TileMap *level;
    std::vector<Object*> objects;
    std::vector<EnemyRed*> enemiesRed;
    std::vector<EnemyBlue*> enemiesBlue;
    std::vector<Explosion*> explosions;


    Camera2D camera;
    DebugMode debug;

    bool game_over = false;
    bool victory = false;
    Point doorPos;
    bool doorHidden;
    bool showStageScreen = false;
    float stageScreenTimer = 0.0f;
    int nextStageToLoad = -1;
};

