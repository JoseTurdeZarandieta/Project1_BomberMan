#include "globals.h"
#include <raylib.h>
#include "game.h"



int main (){

    Game *game;
    AppStatus status;
    int main_return = EXIT_SUCCESS;

    LOG("Application start");
    game = new Game();
    status = game->Initialise(GAME_SCALE);
    
    if (status != AppStatus::OK)
    {
        LOG("Failed to initialise game");
        main_return = EXIT_FAILURE;
    }
    while (status == AppStatus::OK)
    {
        status = game->Update();
        if(status != AppStatus::OK)
        {
            if(status == AppStatus::ERROR)      main_return = EXIT_FAILURE;
            else if(status == AppStatus::QUIT)  main_return = EXIT_SUCCESS;
            break;
        }
        game->Render();
    }

    LOG("Application finish");
    game->Cleanup();
    
    LOG("Bye :)");
    delete game;
   
    return main_return;
}