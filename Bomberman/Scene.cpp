#include "Globals.h"
#include "Scene.h"



Scene::Scene()
{
	player = nullptr;
    level = nullptr;
	
	camera.target = { 0, 0 };				//Center of the screen
	camera.offset = { 0, MARGIN_GUI_Y };	//Offset from the target (center of the screen)
	camera.rotation = 0.0f;					//No rotation
	camera.zoom = 1.0f;						//Default zoom

	debug = DebugMode::OFF;
}
Scene::~Scene()
{
	if (player != nullptr)
	{
		player->Release();
		delete player;
		player = nullptr;
	}
    if (level != nullptr)
    {
		level->Release();
        delete level;
        level = nullptr;
    }
	for (Entity* obj : objects)
	{
		delete obj;
	}
	objects.clear();
}
AppStatus Scene::Init()
{
	//Create player
	player = new Player({ 0,0 }, State::IDLE, Look::RIGHT);
	if (player == nullptr)
	{
		LOG("Failed to allocate memory for Player");
		return AppStatus::ERROR;
	}
	//Initialise player
	if (player->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Player");
		return AppStatus::ERROR;
	}

	//Create level 
    level = new TileMap();
    if (level == nullptr)
    {
        LOG("Failed to allocate memory for Level");
        return AppStatus::ERROR;
    }
	//Initialise level
	if (level->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Level");
		return AppStatus::ERROR;
	}
	//Load level
	if (LoadLevel(1) != AppStatus::OK)
	{
		LOG("Failed to load Level 1");
		return AppStatus::ERROR;
	}
	//Assign the tile map reference to the player to check collisions while navigating
	player->SetTileMap(level);

    return AppStatus::OK;
}


AppStatus Scene:: LoadLevel(int stage){

	int size;
	int x,y,i=0;
	Tile tile;
	int *map = nullptr;
	Object *obj;
	AABB hitbox,area;


	ClearLevel();

	size = LEVEL_WIDTH * LEVEL_HEIGHT;
	if(stage==1){
		map = new int [size]{


		};
		player-> InitScore();//iniciar tiempo,vidas  y vidas aqui 

	} else if(stage==2){
		map = new int [size]{


		};
	




	}else if(stage==3){
		map = new int [size]{


		};
	




	}else if(stage==4){
		map = new int [size]{


	};
	




}else {
	LOG("Failed to loaf level, stage %d doesn't exist", stage);
	return AppStatus:: ERROR;

}

level->Load(map,LEVEL_WIDTH	,LEVEL_HEIGHT);
for(y=0;y<LEVEL_HEIGHT,++y){
	for(x=0;x<LEVEL_WIDTH,++x){
		tile= (TILE)map[i];
		if (level->IsTileEntity(tile)|| level->IsTileObject(tile)){
			pos.x = x * TILE_SIZE;
			pos.y = y * TILE_SIZE + TILE_SIZE - 1;

			if (tile== TILE::PLAYER){
				player-> SetPos(pos);
			}
			else if (tile==TILE::ITEM_)/*nombre del item*/{
				obj = new Object(pos,ObjectType::/*nombre del item*/)
				objects.push_back(obj);
			}else{
				LOG("Internal error loading scene: invalid entity or object tile id")
			}
		}
		++i

	}

}

level->ClearObjectEntityPositions();

delete[]map;

return AppStatus:: OK;

}

void Scene:: Update(){

	Point p1,p2;
	AABB hitBox;

	if (IsKeyPressed(KEY_F1)){
		debug =(DebugMode)(((int)debug+1)%(int)DebugMode::SIZE)

	}
	if (IsKeyPressed(KEY_ONE)){
		LoadLevel(1);
	} else if (IsKeyPressed(KEY_TWO)){
		LoadLevel(2);
	}

	level->Update();
	player->Update();
	CheckObjectCollisions();

	hitbox = player-> GetHitbox();
	enemies -> Update();
	shots -> Update(hitbox);
	paritcles-> Update();

}

void Scene::Render(){


}


void Scene::Release(){

}


void Scene::CheckObjectCollisions(){

}


void Scene::ClearLevel(){

}

void Scene::RenderObjects()const {
 for ()

}

void Scene::RenderObjectsDebug()const {


}

void Scene:: RenderGUI(){


}
