



GAME::Game(){
    state=GameState::Main_menu;
    scene= nullptr;
    img_menu= nullptr;

    target= {};
    src = {};
    dst= {};
}
GAME::~Game(){
    if(scene != nullptr){
        scene->Release(); 
        delete scene;
        scene = nullptr;

    }

}






AppStatus Game:: initialise(float scale){

    float w, h;
    w = WINDOW_WIDTH * scale;
    h = WINDOW_HEIGHT * scale;

    initWindow((int)w,(int)h, "Bomberman");

    target = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (target.id == 0)
    {
        LOG("Failed to create render texture");
        return AppStatus::ERROR;
    }
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    src={0,0,WINDOW_WIDTH,WINDOW_HEIGHT}
    dst={0,0,w,h};
}

AppStatus Game:: Update(){
switch (State) {
    case GameState::Main_menu:
        if (IsKeyPressed(KEY_ESCAPE)){
                return AppStatus::QUIT;
        } 
        if (IsKeyPressed(KEY_ENTER)){
            if(BeginPlay() != AppStatus::OK) return AppStatus::ERROR;
            state = GameState::PLAYING;

        }
    case Playing:
        if (IsKeyPressed(KEY_ESCAPE)){
               FinishPlay();
               state= GameState::Main_menu;
        } else {
            scene->Update();

        }
        break;
    }
    return AppStatus::OK;

    if(LoadResources() != AppStatus::OK){
        LOG("Failed to load resources");
        return AppStatus:: ERROR;
    }

    SetTargetFPS(60);
    SetExitKey(0);
    return AppStatus::OK;

}

AppStatus Game::LoadResources(){

    ResourceManager& data = ResourceManager::Instance();

    //Hay que mirarse esto
    if (data.LoadTexture(Resource::/*IMG_MENU*/, /*"images/menu.png"*/) != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_menu = data.GetTexture(Resource::IMG_MENU);
    
    return AppStatus::OK;
}

AppStatus Game::BeginPlay(){
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

void Game::Render(){
 
 BeginTextureMode(target);
 ClearBackground(BLACK);
 
 switch (state)
 {
     case GameState::MAIN_MENU:
         DrawTexture(*img_menu, 0, 0, WHITE);
         break;

     case GameState::PLAYING:
         scene->Render();
         break;
 }
 
 EndTextureMode();

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