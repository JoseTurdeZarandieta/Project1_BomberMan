#pragma once

//Displaying scale factor
#define GAME_SCALE_FACTOR	2.0f

//Function return values
enum class AppStatus { OK = 0, ERROR, QUIT };

//Main return values
#define EXIT_SUCCESS	0
#define EXIT_FAILURE	1

//Log messages
#define LOG(text, ...) log(__FILE__, __LINE__, text, __VA_ARGS__);
void log(const char file[], int line, const char* text, ...);

//Window size
#define WINDOW_WIDTH    400
#define WINDOW_HEIGHT   300

//Game constants
#define MARGIN_GUI_Y	28
#define TILE_SIZE		16
#define LEVEL_WIDTH		31
#define LEVEL_HEIGHT	14

//The upper bar is one and a half tile 
//map is 16 width and 14 and a half width 

//Entities animation delay
#define ANIM_DELAY		4