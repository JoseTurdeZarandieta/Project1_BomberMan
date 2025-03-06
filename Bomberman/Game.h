#include "Globals.h"


enum class GameState {Main_menu, Playing, Credits};


class Game {
public:
    Game();
    ~Game();
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

    RenderTexture2D target;
    Rectangle src, dst;
};

