#include "AudioManager.h"
#include "Scene.h"
#include <stdio.h>
#include "Globals.h"
#include "Entity.h"
#include "Enemy.h"
#include "player.h"
#include <fstream>
#include <sstream>
#include <vector>

Scene::Scene()
{
	player = nullptr;
    level = nullptr;
	
	camera.target = { 0, 0 };
	camera.offset = { 0, MARGIN_GUI_Y };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

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
	
	for (Entity* enem : enemies)
	{
		delete enem;
	}
	enemies.clear();
}
AppStatus Scene::Init()
{
	//Create player
	player = new Player({ 16,32 }, State::IDLE, Look::RIGHT);
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

	//create and initialise enemy
	Enemy* enemy = new Enemy({ 176, 192 });
	enemy->SetTileMap(level);
	if (enemy->Initialise() != AppStatus::OK) {
		return AppStatus::ERROR;
	}
	enemies.push_back(enemy);

    return AppStatus::OK;
}
AppStatus Scene::LoadLevel(int stage)
{
	int x, y, i;
	Tile tile;
	Point pos;
	Object* obj;

	ClearLevel();

	int size = LEVEL_WIDTH * LEVEL_HEIGHT;
	int* map = new int[size];

	std::string filename = "level" + std::to_string(stage) + ".txt";
	if (!LoadMapFromFile(filename, map, size)) {
		LOG("Failed to load level file: %s", filename.c_str());
		delete[] map;
		return AppStatus::ERROR;
	}

	// --- POSICIÓN DE LA PUERTA SEGÚN EL NIVEL ---
	int doorX = 5, doorY = 5; // Por defecto
	switch (stage) {
	case 1: doorX = 3;  doorY = 3;  break;
	case 2: doorX = 10; doorY = 7;  break;
	case 3: doorX = 19; doorY = 10; break;
	case 4: doorX = 25; doorY = 5; break;
		// Añade más casos según tus niveles
	default: doorX = 5; doorY = 5; break;
	}
	int doorIndex = doorY * LEVEL_WIDTH + doorX;
	map[doorIndex] = static_cast<int>(Tile::SOFT_BLOCK);
	doorPos = { doorX, doorY };
	doorHidden = true;

	// --- PROCESA EL MAPA Y COLOCA OBJETOS/JUGADOR ---
	i = 0;
	for (y = 0; y < LEVEL_HEIGHT; ++y)
	{
		for (x = 0; x < LEVEL_WIDTH; ++x)
		{
			tile = (Tile)map[i];
			if (tile == Tile::EMPTY)
			{
				map[i] = 0;
			}
			else if (tile == Tile::PLAYER)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				player->SetPos(pos);
				map[i] = 0;
			}
			else if (tile == Tile::ITEM_BOMB_UP)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				obj = new Object(pos, ObjectType::BOMB_UP);
				objects.push_back(obj);
				map[i] = 0;
			}
			else if (tile == Tile::ITEM_FIRE_UP)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				obj = new Object(pos, ObjectType::FIRE_UP);
				objects.push_back(obj);
				map[i] = 0;
			}
			++i;
		}
	}

	level->Load(map, LEVEL_WIDTH, LEVEL_HEIGHT);
	delete[] map;

	return AppStatus::OK;
}


bool Scene::LoadMapFromFile(const std::string& filename, int* map, int size)
{
	std::ifstream file(filename);
	if (!file.is_open()) {
		LOG("No se pudo abrir el archivo: %s", filename.c_str());
		return false;
	}

	int value, count = 0;
	while (file >> value && count < size) {
		map[count++] = value;
	}
	file.close();
	if (count != size) {
		LOG("El archivo %s no tiene el nÃºmero correcto de tiles (%d/%d)", filename.c_str(), count, size);
		return false;
	}
	return true;
}
void Scene::Update()
{
	Point p1, p2;
	AABB box;

	//Switch between the different debug modes: off, on (sprites & hitboxes), on (hitboxes) 
	if (IsKeyPressed(KEY_F1))
	{
		debug = (DebugMode)(((int)debug + 1) % (int)DebugMode::SIZE);
	}
	else if (IsKeyPressed(KEY_F2))
	{
		victory = true;
	}
	else if (IsKeyPressed(KEY_F3))
	{
		game_over = true;
	}
	if (IsKeyPressed(KEY_F4))
	{
		player->health -= 1;
	}
	if (player->GetHealth() <= 0)
	{
		game_over = true;

	}
	//Debug levels instantly
	if (IsKeyPressed(KEY_ONE))		LoadLevel(1);
	else if (IsKeyPressed(KEY_TWO))	LoadLevel(2);
	else if (IsKeyPressed(KEY_THREE))	LoadLevel(3);
	else if (IsKeyPressed(KEY_FOUR))	LoadLevel(4);
	else if (IsKeyPressed(KEY_FIVE))	LoadLevel(5);
	else if (IsKeyPressed(KEY_SIX))	LoadLevel(6);
	/*else if (player->NextLevel() && currentstage == 1)
	{
		LoadLevel(2);

		player->SetPos({ 20, player->GetY() });
	}
	else if (player->PrevLevel() && currentstage == 2)
	{
		LoadLevel(1);

		player->SetPos({ 390, player->GetY() });
	}*/
	else if (player->NextLevel() && currentstage == 3)
	{
		LoadLevel(4);

		player->SetPos({ 20, player->GetY() });
	}
	else if (player->PrevLevel() && currentstage == 4)
	{
		LoadLevel(3);

		player->SetPos({ 390, player->GetY() });
	}
	else if (player->NextLevel() && currentstage == 5)
	{
		LoadLevel(6);

		player->SetPos({ 20, player->GetY() });
	}
	else if (player->PrevLevel() && currentstage == 6)
	{
		LoadLevel(5);

		player->SetPos({ 390, player->GetY() });
	}
	else if (player->NextLevel() && currentstage == 7)
	{
		LoadLevel(8);

		player->SetPos({ 20, player->GetY() });
	}
	else if (player->PrevLevel() && currentstage == 8)
	{
		LoadLevel(7);

		player->SetPos({ 390, player->GetY() });
	}
	if (player->bombCooldown > 0.0f) {
		player->bombCooldown -= GetFrameTime();
		if (player->bombCooldown < 0.0f) player->bombCooldown = 0.0f;
	}

	if (IsKeyPressed(KEY_D) && player->activeBombs.empty() && player->bombCooldown <= 0.0f)
	{
		Point bombPos = player->GetPos();
		int tileX = bombPos.x / TILE_SIZE;
		int tileY = bombPos.y / TILE_SIZE;
		int tileIndex = tileY * LEVEL_WIDTH + tileX;

		// Solo colocar bomba si la tile está vacía
		if (level->map[tileIndex] == Tile::AIR)
		{
			AudioManager::Instance().PlaySoundByName("BombDown");
			level->map[tileIndex] = Tile::BOMB;
			Player::Bomb newBomb = { bombPos, 0.0f };
			player->activeBombs.push_back(newBomb);
			player->bombCooldown = 0.2f; // Pequeño cooldown para evitar doble pulsación
		}
	}


	for (int i = 0; i < player->activeBombs.size();)
	{
		player->activeBombs[i].timer += GetFrameTime();

		if (player->activeBombs[i].timer > 3.0f) {
			AudioManager::Instance().PlaySoundByName("BombExplode");
			// Calcular la posición de las tiles adyacentes al jugador
			int tileX = player->activeBombs[i].position.x / TILE_SIZE; // Asumiendo que TILE_SIZE es el tamaño de una tile
			int tileY = player->activeBombs[i].position.y / TILE_SIZE;
			// Modificar el valor de las tiles en todas las direcciones a 0, incluyendo la posición del jugador
			// Afecta la propia bomba primero
			int centerTileIndex = tileY * LEVEL_WIDTH + tileX;
			if (level->map[centerTileIndex] != Tile::BLOCK)
			{
				if (doorHidden && tileX == doorPos.x && tileY == doorPos.y && level->map[centerTileIndex] == Tile::SOFT_BLOCK)
				{
					level->map[centerTileIndex] = Tile::DOOR;
					doorHidden = false;
				}
				else
				{
					level->map[centerTileIndex] = Tile::AIR;
				}
			}

			// Direcciones: derecha, izquierda, abajo, arriba
			const int directions[4][2] = {
				{1, 0},   // derecha
				{-1, 0},  // izquierda
				{0, 1},   // abajo
				{0, -1}   // arriba
			};

			for (int d = 0; d < 4; ++d)
			{
				int dx = directions[d][0];
				int dy = directions[d][1];

				for (int step = 1; step <= 2; ++step)
				{
					int adjTileX = tileX + dx * step;
					int adjTileY = tileY + dy * step;

					// Verifica que esté dentro del mapa
					if (adjTileX < 0 || adjTileX >= LEVEL_WIDTH || adjTileY < 0 || adjTileY >= LEVEL_HEIGHT)
						break;

					int tileIndex = adjTileY * LEVEL_WIDTH + adjTileX;

					if (level->map[tileIndex] == Tile::BLOCK)
					{
						// Bloque sólido: detener explosión en esta dirección
						break;
					}

					if (doorHidden && adjTileX == doorPos.x && adjTileY == doorPos.y && level->map[tileIndex] == Tile::SOFT_BLOCK)
					{
						level->map[tileIndex] = Tile::DOOR;
						doorHidden = false;
						break; // También detener explosión después de revelar puerta
					}
					else
					{
						level->map[tileIndex] = Tile::AIR;
					}

					if (level->map[tileIndex] == Tile::SOFT_BLOCK)
					{
						// Detener explosión si destruye un soft block
						break;
					}
				}
			}


			int playerTileX = player->GetX() / TILE_SIZE;
			int playerTileY = player->GetY() / TILE_SIZE;

			// Solo daña si está alineado horizontal o verticalmente y a distancia de 0 a 2 tiles
			int dx = std::abs(playerTileX - tileX);
			int dy = std::abs(playerTileY - tileY);

			if ((dx == 0 && dy <= 2) || (dy == 0 && dx <= 2))
			{
				player->takeDamage(1);
				if (player->GetHealth() <= 0)
				{
					game_over = true;
				}
			}


			player->activeBombs.erase(player->activeBombs.begin() + i);


		}
		else
		{
			++i;
		}
	}
	if (player->victory)
	{
		player->victory = false;
		int nextStage = currentstage + 1;
		if (LoadLevel(nextStage) == AppStatus::OK) {
			currentstage = nextStage;
			
			player->SetPos({16,32});
		}
		else {
			victory = true;
		}
	}

	level->Update();

	player->Update();
	for (Enemy* enemy : enemies)
		enemy->Update();
	CheckCollisions();

	//start of camera following player
	Point center = player->GetPos();
	camera.target = { static_cast<float>(center.x), static_cast<float>(center.y) };

	int mapWidthInPixels = level->width * TILE_SIZE;
	int mapHeightInPixels = level->height * TILE_SIZE;

	float halfScreenWidth = WINDOW_WIDTH / 4.0f;
	float halfScreenHeight = WINDOW_HEIGHT / 6.0f; //change to 2.0f, at 4.0f is to check that it works

	float posCamX = std::clamp(camera.target.x, halfScreenWidth, mapWidthInPixels - halfScreenWidth);
	float posCamY = 0;

	camera.target = { posCamX, posCamY };
	camera.offset = { halfScreenWidth, halfScreenHeight };

	//end of camera following player

}
void Scene::Render()
{
	BeginMode2D(camera);

    level->Render();
	if (debug == DebugMode::OFF || debug == DebugMode::SPRITES_AND_HITBOXES)
	{
		RenderObjects(); 
		player->Draw();
		for (Enemy* enemy : enemies)
			enemy->Draw();
	}
	if (debug == DebugMode::SPRITES_AND_HITBOXES || debug == DebugMode::ONLY_HITBOXES)
	{
		RenderObjectsDebug(YELLOW);
		player->DrawDebug(GREEN);
	}

	EndMode2D();

	RenderGUI();
}
void Scene::Release()
{
    level->Release();
	player->Release();
	ClearLevel();
}
void Scene::CheckCollisions()
{
	AABB player_box, obj_box;
	
	player_box = player->GetHitbox();
	auto it = objects.begin();
	while (it != objects.end())
	{
		obj_box = (*it)->GetHitbox();
		if(player_box.TestAABB(obj_box))
		{
			player->IncrScore((*it)->Points());
			
			//Delete the object
			delete* it; 
			//Erase the object from the vector and get the iterator to the next valid element
			it = objects.erase(it); 
		}
		else
		{
			//Move to the next object
			++it; 
		}
	}
}
void Scene::ClearLevel()
{
	for (Object* obj : objects)
	{
		delete obj;
	}
	objects.clear();
	for (Enemy* enem : enemies)
	{
		delete enem;
	}
	enemies.clear();
}
void Scene::RenderObjects() const
{
	for (Object* obj : objects)
	{
		obj->Draw();
	}
}
void Scene::RenderObjectsDebug(const Color& col) const
{
	for (Object* obj : objects)
	{
		obj->DrawDebug(col);
	}
}
void Scene::RenderGUI() const
{
	//Temporal approach
	DrawText(TextFormat("SCORE : %d", player->GetScore()), 10, 10, 8, LIGHTGRAY);
	DrawText(TextFormat("HEALTH : %d", player->GetHealth()), 325, 10, 8, LIGHTGRAY);
}