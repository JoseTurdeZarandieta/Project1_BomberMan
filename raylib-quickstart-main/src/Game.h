#pragma once
#include "Globals.h"
#include "Scene.h"


enum class GameState { SCREEN1, SCREEN2, MAIN_MENU, PLAYING, SETTINGS, CREDITS ,WIN, LOSE};

class Game
{
public:
    Game();
    ~Game();

    AppStatus Initialise(float scale);
    AppStatus Update();
    void Render();
    
    void Cleanup();

private:
    AppStatus BeginPlay();
    void FinishPlay();

    AppStatus LoadResources();
    void UnloadResources();

    GameState state;
    Scene *scene;
    const Texture2D *img_menu;
    const Texture2D* img_screen1;
    const Texture2D* img_screen2;
    const Texture2D* img_win;
    const Texture2D* img_lose;
    const Texture2D* img_items;

    //To work with original game units and then scale the result
    RenderTexture2D target;
    Rectangle src, dst;
};