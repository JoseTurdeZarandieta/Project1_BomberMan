#include "Game.h"
#include "Globals.h"
#include "ResourceManager.h"
#include <stdio.h>

Game::Game()
{
    state = GameState::SCREEN1;
    scene = nullptr;
    img_menu = nullptr;
    img_screen1 = nullptr;
    img_screen2 = nullptr;
    img_win = nullptr;
    img_lose = nullptr;

    target = {};
    src = {};
    dst = {};
}
Game::~Game()
{
    if (scene != nullptr)
    {
        scene->Release();
        delete scene;
        scene = nullptr;
    }
}
AppStatus Game::Initialise(float scale)
{
    float w, h;
    w = WINDOW_WIDTH * scale;
    h = WINDOW_HEIGHT * scale;

    //Initialise window
    InitWindow((int)w, (int)h, "Bomberman");

    //Render texture initialisation, used to hold the rendering result so we can easily resize it
    target = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (target.id == 0)
    {
        LOG("Failed to create render texture");
        return AppStatus::ERROR;
    }
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    src = { 0, 0, WINDOW_WIDTH, -WINDOW_HEIGHT };
    dst = { 0, 0, w, h };

    //Load resources
    if (LoadResources() != AppStatus::OK)
    {
        LOG("Failed to load resources");
        return AppStatus::ERROR;
    }

    //Set the target frame rate for the application
    SetTargetFPS(60);
    //Disable the escape key to quit functionality
    SetExitKey(0);

    return AppStatus::OK;
}
AppStatus Game::LoadResources()
{
    ResourceManager& data = ResourceManager::Instance();
    
    if (data.LoadTexture(Resource::IMG_SCREEN1, "resources/UI/BombermanScreen1.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_screen1 = data.GetTexture(Resource::IMG_SCREEN1);

    if (data.LoadTexture(Resource::IMG_SCREEN2, "resources/UI/BombermanScreen2.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_screen2 = data.GetTexture(Resource::IMG_SCREEN2);

    if (data.LoadTexture(Resource::IMG_MENU, "resources/UI/Title.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_menu = data.GetTexture(Resource::IMG_MENU);
    if (data.LoadTexture(Resource::IMG_WIN, "resources/UI/Win.jpg") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_win = data.GetTexture(Resource::IMG_WIN);

    if (data.LoadTexture(Resource::IMG_LOSE, "resources/UI/GameOver.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_lose = data.GetTexture(Resource::IMG_LOSE);
    
    
    return AppStatus::OK;
}
AppStatus Game::BeginPlay()
{
    scene = new Scene();
    if (scene == nullptr)
    {
        LOG("Failed to allocate memory for Scene");
        return AppStatus::ERROR;
    }
    if (scene->Init() != AppStatus::OK)
    {
        LOG("Failed to initialise Scene");
        return AppStatus::ERROR;
    }

    return AppStatus::OK;
}
void Game::FinishPlay()
{
    scene->Release();
    delete scene;
    scene = nullptr;
}
AppStatus Game::Update()
{
    //Check if user attempts to close the window, either by clicking the close button or by pressing Alt+F4
    if(WindowShouldClose()) return AppStatus::QUIT;

    switch (state)
    {
    case GameState::SCREEN1:
        if (IsKeyPressed(KEY_SPACE))
        {
            state = GameState::SCREEN2;
        }
        break;

    case GameState::SCREEN2:
        if (IsKeyPressed(KEY_SPACE))
        {
            state = GameState::MAIN_MENU;
        }
        break;

    case GameState::MAIN_MENU:
        if (IsKeyPressed(KEY_SPACE))
        {
            if (BeginPlay() != AppStatus::OK) return AppStatus::ERROR;
            state = GameState::PLAYING;
        }
        break;

    case GameState::PLAYING:
        if (IsKeyPressed(KEY_ESCAPE))
        {
            FinishPlay();
            state = GameState::MAIN_MENU;
        }
        else if (scene->game_over)
        {
            state = GameState::LOSE;
        }
        else if (scene->victory)
        {
            state = GameState::WIN;
        }
        {
            scene->Update();
        }
        break;
    case GameState::WIN:
        if (IsKeyPressed(KEY_SPACE))
        {
            FinishPlay();
            state = GameState::MAIN_MENU;
        }
        break;
    case GameState::LOSE:
        if (IsKeyPressed(KEY_SPACE))
        {
            FinishPlay();
            state = GameState::MAIN_MENU;
        }
        break;
    }
    return AppStatus::OK;
}
void Game::Render()
{
    BeginTextureMode(target);
    ClearBackground(DARKGREEN);

    switch (state)
    {
    case GameState::SCREEN1:
        DrawTexture(*img_screen1, 0, 0, WHITE);
        break;
    case GameState::SCREEN2:
        DrawTexture(*img_screen2, 0, 0, WHITE);
        break;
    case GameState::MAIN_MENU:
        DrawTexture(*img_menu, 0, 0, WHITE);
        break;
    case GameState::PLAYING:
        scene->Render();
        break;
    case GameState::WIN:
        DrawTexture(*img_win, 0, 0, WHITE);
        break;
    case GameState::LOSE:
        DrawTexture(*img_lose, 0, 0, WHITE);
    }

    
    EndTextureMode();

    //Draw render texture to screen, properly scaled
    BeginDrawing();
    DrawTexturePro(target.texture, src, dst, { 0, 0 }, 0.0f, WHITE);
    EndDrawing();
}
void Game::Cleanup()
{
    UnloadResources();
    CloseWindow();
}
void Game::UnloadResources()
{
    ResourceManager& data = ResourceManager::Instance();
    data.ReleaseTexture(Resource::IMG_MENU);

    UnloadRenderTexture(target);
}