#include "AudioManager.h"
#include "Scene.h"
#include <stdio.h>
#include "Globals.h"
#include "Entity.h"
#include "EnemyRed.h"
#include "EnemyBlue.h"
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
	
	for (Entity* enemyRed : enemiesRed)
	{
		delete enemyRed;
	}
	enemiesRed.clear();
	for (Entity* enemyBlue : enemiesBlue)
	{
		delete enemyBlue;
	}
	enemiesBlue.clear();
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

	showStageScreen = true;
	stageScreenTimer = 2.0f;
	nextStageToLoad = 1;
	currentstage = 1;

	//Assign the tile map reference to the player to check collisions while navigating
	player->SetTileMap(level);

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

	int doorX = 5, doorY = 5; // Por defecto
	int PowerUpX = 3, PowerUpY = 3;
	switch (stage) {
	case 1: doorX = 3;  doorY = 3; PowerUpX = 4; PowerUpY = 7; break;
	case 2: doorX = 10; doorY = 7;  PowerUpX = 3; PowerUpY = 4; break;
	case 3: doorX = 19; doorY = 10; PowerUpX = 5; PowerUpY = 5; break;
	case 4: doorX = 25; doorY = 5; PowerUpX = 3; PowerUpY = 3; break;

	default: doorX = 5; doorY = 5; PowerUpX = 3; PowerUpY = 3; break;
	}
	int doorIndex = doorY * LEVEL_WIDTH + doorX;
	int powerUpIndex = PowerUpY * LEVEL_WIDTH + PowerUpX;
	map[doorIndex] = static_cast<int>(Tile::SOFT_BLOCK);
	map[powerUpIndex] = static_cast<int>(Tile::SOFT_BLOCK);

	doorPos = { doorX, doorY };
	doorHidden = true;
	powerUpPos = { PowerUpX, PowerUpY };
	powerUpHidden = true;


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
			else if (tile == Tile::ENEMY_RED)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE;
				EnemyRed* enemyRed = new EnemyRed(pos);
				enemyRed->SetTileMap(level);
				if (enemyRed->Initialise() == AppStatus::OK) {
					enemiesRed.push_back(enemyRed);
				}
				else {
					LOG("Failed to initialise enemy at (%d, &d)", pos.x, pos.y);
					delete enemyRed;
				}
				map[i] = 0; //when enemy moves, it places AIR
			}
			else if (tile == Tile::ENEMY_BLUE)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE;
				EnemyBlue* enemyBlue = new EnemyBlue(pos);
				enemyBlue->SetTileMap(level);
				if (enemyBlue->Initialise() == AppStatus::OK) {
					enemiesBlue.push_back(enemyBlue);
				}
				else {
					LOG("Failed to initialise enemy at (%d, &d)", pos.x, pos.y);
					delete enemyBlue;
				}
				map[i] = 0; //when enemy moves, it places AIR
			}

			++i;
		}
	}

	level->Load(map, LEVEL_WIDTH, LEVEL_HEIGHT);

	stageTimer = 0.0f;
	startTimer = true;

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
		LOG("El archivo %s no tiene el número correcto de tiles (%d/%d)", filename.c_str(), count, size);
		return false;
	}
	return true;
}
void Scene::Update()
{
	if (showStageScreen)
	{
		stageScreenTimer -= GetFrameTime();
		if (stageScreenTimer <= 0.0f)
		{
			showStageScreen = false;
			if (LoadLevel(nextStageToLoad) == AppStatus::OK) {
				currentstage = nextStageToLoad;

				player->SetPos({ 16,32 });			
			}
			else {
				victory = true;
			}
		}
		return;
	}

	if (startTimer) {
		stageTimer += GetFrameTime();
		if (stageTimer >= 200.0f) {
			
			startTimer = false;
			
			player->SetPos({ level1PlayerPos });
			player->maxBombs = initialMaxBombs;
			player->fire_range = initialFireRange;
			player->PLAYER_SPEED = initialSpeed;
			player->remotecontrol = initialRemoteControl;

			LoadLevel(1);
			currentstage = 1;
		}
	}

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
	if (IsKeyPressed(KEY_ONE)) {
		LoadLevel(1);
		currentstage = 1;
	}
	else if (IsKeyPressed(KEY_TWO)) {
		LoadLevel(2);
		currentstage = 2;
	}
	else if (IsKeyPressed(KEY_THREE)) {
		LoadLevel(3);
		currentstage = 3;
	}
	else if (IsKeyPressed(KEY_FOUR)) {
		LoadLevel(4);
		currentstage = 4;
	}
	else if (IsKeyPressed(KEY_FIVE)) {
		LoadLevel(5);
		currentstage = 5;
	}
	else if (IsKeyPressed(KEY_SIX)) {
		LoadLevel(6);
		currentstage = 6;
	}

	else if (player->NextLevel() && currentstage == 1)
	{
		LoadLevel(2);

		player->SetPos({ 20, player->GetY() });
	}
	else if (player->PrevLevel() && currentstage == 2)
	{
		LoadLevel(1);

		player->SetPos({ 390, player->GetY() });
	}
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

	if (IsKeyPressed(KEY_D) && player->activeBombs.size() < player->maxBombs && player->bombCooldown <= 0.0f)
	{
		Point bombPos = player->GetPos();
		int tileX = bombPos.x / TILE_SIZE;
		int tileY = bombPos.y / TILE_SIZE;
		int tileIndex = tileY * LEVEL_WIDTH + tileX;

		if (level->map[tileIndex] == Tile::AIR)
		{
			AudioManager::Instance().PlaySoundByName("BombDown");
			level->map[tileIndex] = Tile::BOMB;

			Player::Bomb newBomb = { bombPos, 0.0f };

			player->activeBombs.push_back(newBomb);
			player->bombCooldown = 0.2f;
		}
	}

	for (int i = 0; i < player->activeBombs.size();)
	{


		player->activeBombs[i].timer += GetFrameTime();

		if (player->remotecontrol && IsKeyPressed(KEY_E)) {
			player->activeBombs[i].timer = 3.5f;
		}

		if (player->activeBombs[i].timer > 3.0f) {
			AudioManager::Instance().PlaySoundByName("BombExplode");
			int tileX = player->activeBombs[i].position.x / TILE_SIZE;
			int tileY = player->activeBombs[i].position.y / TILE_SIZE;

			/*		bomb logic		*/
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

			const int directions[4][2] = { { 1,  0}, {-1,  0}, { 0,  1}, { 0, -1} };
			const Point armDirs[4] = { { 1,  0},{-1,  0},{ 0,  1}, { 0, -1} };
			const explosionAnim endTypes[4] = {
				explosionAnim::EXPLOSION_RIGHT,
				explosionAnim::EXPLOSION_LEFT,
				explosionAnim::EXPLOSION_DOWN,
				explosionAnim::EXPLOSION_UP
			};

			/*range of explosion*/
			int range = player->fire_range;
			for (int d = 0; d < 4; ++d) {
				int dirX = directions[d][0], dirY = directions[d][1];
				for (int step = 1; step <= range; ++step) {
					int adjTileX = tileX + dirX * step;
					int adjTileY = tileY + dirY * step;
					if (adjTileX < 0 || adjTileX >= LEVEL_WIDTH || adjTileY < 0 || adjTileY >= LEVEL_HEIGHT)
						break;
					int idTile = adjTileY * LEVEL_WIDTH + adjTileX;
					Tile bombedTile = level->map[idTile];
					if (bombedTile == Tile::BLOCK || bombedTile == Tile::DOOR || bombedTile == Tile::BOMB)
						break;

					int tileIndex = adjTileY * LEVEL_WIDTH + adjTileX;
					if (level->map[tileIndex] == Tile::BLOCK || level->map[tileIndex] == Tile::DOOR || level->map[tileIndex] == Tile::ITEM_BOMB_UP
						|| level->map[tileIndex] == Tile::ITEM_FIRE_UP || level->map[tileIndex] == Tile::ITEM_SPEED_UP || level->map[tileIndex] == Tile::ITEM_REMOTE_CONTROL) 
					{
						break;
					}
					if (doorHidden && adjTileX == doorPos.x && adjTileY == doorPos.y && level->map[tileIndex] == Tile::SOFT_BLOCK)
					{
						level->map[tileIndex] = Tile::DOOR;
						doorHidden = false;
						break;
					}
					else if (powerUpHidden && adjTileX == powerUpPos.x && adjTileY == powerUpPos.y && level->map[tileIndex] == Tile::SOFT_BLOCK) {
						if (currentstage == 1) {
							level->map[tileIndex] = Tile::ITEM_BOMB_UP;
						}
						else if (currentstage == 2) {
							level->map[tileIndex] = Tile::ITEM_FIRE_UP;
						}
						else if (currentstage == 3) {
							level->map[tileIndex] = Tile::ITEM_SPEED_UP;
						}
						else if (currentstage == 4) {
							level->map[tileIndex] = Tile::ITEM_REMOTE_CONTROL;
						}
						powerUpHidden = false;
						break;
					}
					else {
						level->map[idTile] = Tile::AIR;
					}
					if (bombedTile == Tile::SOFT_BLOCK) {
						break;

					}
				}
			}

			/*		bomb	&	anim	*/
			const Point bombPos = player->activeBombs[i].position;
			int newtileX = bombPos.x / TILE_SIZE;
			int newtileY = bombPos.y / TILE_SIZE;
			Point center = { newtileX * TILE_SIZE, newtileY * TILE_SIZE };

			auto* explosion = new Explosion(center, explosionAnim::EXPLOSION_CENTER);
			if (explosion->Initialise() == AppStatus::OK) {
				explosions.emplace_back(explosion);
			}

			for (int d = 0; d < 4; ++d) {
				int dirX = directions[d][0], dirY = directions[d][1];
				int endStep = 0;

				for (int step = 1; step <= range; ++step) {
					int x = tileX + dirX * step;
					int y = tileY + dirY * step;
					if (x < 0 || x >= LEVEL_WIDTH || y < 0 || y >= LEVEL_HEIGHT) {
						break;
					}
					Tile bombedTile = level->map[y * LEVEL_WIDTH + x];
					if (bombedTile != Tile::AIR && bombedTile != Tile::DOOR) {
						break;
					}
					endStep = step;
				}
				if (endStep == 0)
					continue;

				for (int step = 1; step < endStep; ++step) {
					Point pos = {
						center.x + armDirs[d].x * TILE_SIZE * step,
						center.y + armDirs[d].y * TILE_SIZE * step
					};
					explosionAnim midType;
					if (d < 2) {
						midType = explosionAnim::EXPLOSION_MID_HOR;
					}
					else {
						midType = explosionAnim::EXPLOSION_MID_VER;
					}
					auto* explosion = new Explosion(pos, midType);
					if (explosion->Initialise() == AppStatus::OK)
						explosions.emplace_back(explosion);
				}

				Point endPos = {
					center.x + armDirs[d].x * TILE_SIZE * endStep,
					center.y + armDirs[d].y * TILE_SIZE * endStep
				};
				explosionAnim endType = endTypes[d];
				auto* explosion = new Explosion(endPos, endType);
				if (explosion->Initialise() == AppStatus::OK)
					explosions.emplace_back(explosion);
			}

			int playerTileX = player->GetX() / TILE_SIZE;
			int playerTileY = player->GetY() / TILE_SIZE;

			int dx = std::abs(playerTileX - tileX);
			int dy = std::abs(playerTileY - tileY);

			if ((dx == 0 && dy <= 1) || (dx <= 1 && dy == 0))
			{
				player->takeDamage(1);
				if (player->GetHealth() <= 0)
				{
					game_over = true;
				}
			}

			// kill enemies
			for (auto m = enemiesRed.begin(); m != enemiesRed.end();) {
				int enemyX = (*m)->GetX() / TILE_SIZE;
				int enemyY = (*m)->GetY() / TILE_SIZE;
				int collEnemX = std::abs(enemyX - tileX);
				int collEnemY = std::abs(enemyY - tileY);
				if ((collEnemX == 1 && collEnemY <= 1 || collEnemX <= 1 && collEnemY == 0)) {
					player->IncrScore(100);
					delete *m;
					m = enemiesRed.erase(m);
					continue;
				}
				++m;
			}
			for (auto m = enemiesBlue.begin(); m != enemiesBlue.end();) {
				int enemyX = (*m)->GetX() / TILE_SIZE;
				int enemyY = (*m)->GetY() / TILE_SIZE;
				int collEnemX = std::abs(enemyX - tileX);
				int collEnemY = std::abs(enemyY - tileY);
				if ((collEnemX == 1 && collEnemY <= 1 || collEnemX <= 1 && collEnemY == 0)) {
					player->IncrScore(200);
					delete* m;
					m = enemiesBlue.erase(m);
					continue;
				}
				++m;
			}
			//end kill enemies

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
		showStageScreen = true;
		stageScreenTimer = 2.0f;
		nextStageToLoad = nextStage;
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

	for (EnemyRed* enemyRed : enemiesRed)
		enemyRed->Update();

	for (EnemyBlue* enemyBlue : enemiesBlue)
		enemyBlue->UpdateBlue();

	for (int i = 0; i < explosions.size();) {
		explosions[i]->Update(GetFrameTime());
		if (explosions[i]->IsFinished()) {
			delete explosions[i];
			explosions.erase(explosions.begin() + i);
		}
		else {
			++i;
		}
	}

	CheckCollisions();

	//start of camera following player
	Point center = player->GetPos();
	camera.target = { static_cast<float>(center.x), static_cast<float>(center.y) };

	int mapWidthInPixels = level->width * TILE_SIZE;
	int mapHeightInPixels = level->height * TILE_SIZE;

	float halfScreenWidth = WINDOW_WIDTH / 4.0f;
	float halfScreenHeight = WINDOW_HEIGHT / 6.0f;

	float posCamX = std::clamp(camera.target.x, halfScreenWidth, mapWidthInPixels - halfScreenWidth);
	float posCamY = 0;

	camera.target = { posCamX, posCamY };
	camera.offset = { halfScreenWidth, halfScreenHeight };

	//end of camera following player

}
void Scene::Render()
{
	if (showStageScreen)
	{
		// Fondo negro
		DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BLACK);
		// Texto centrado
		std::string stageText = "STAGE " + std::to_string(nextStageToLoad);
		int fontSize = 40;
		int textWidth = MeasureText(stageText.c_str(), fontSize);
		DrawText(stageText.c_str(), (WINDOW_WIDTH - textWidth) / 2, WINDOW_HEIGHT / 2 - fontSize / 2, fontSize, WHITE);
		return;
	}

	BeginMode2D(camera);

    level->Render();
	for (auto* explosion : explosions) {
		explosion->Draw();
	}
	if (debug == DebugMode::OFF || debug == DebugMode::SPRITES_AND_HITBOXES)
	{
		RenderObjects();
		player->Draw();
		for (EnemyRed* enemyRed : enemiesRed) {
			enemyRed->Draw();
		}
		for (EnemyBlue* enemyBlue : enemiesBlue) {
			enemyBlue->DrawBlue();
		}
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
		if (player_box.TestAABB(obj_box))
		{
			player->IncrScore((*it)->Points());

			delete* it;
			it = objects.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (EnemyRed* enemy : enemiesRed) {
		float timeFrameTime = player->invincibiltyTimer;
		float speed = player->invincibleSpeed;
		if (player_box.TestAABB(enemy->GetHitbox())) {

			if (!player->isPlayerInvincible) {
				player->takeDamage(1);
				player->isPlayerInvincible = true;
				speed = player->PLAYER_SPEED;
				player->PLAYER_SPEED = 0.0f;
				timeFrameTime = stageTimer;
			}
			player->invincibiltyTimer = timeFrameTime;
			player->invincibleSpeed = speed;
		}
		if (player->isPlayerInvincible) {
			if (stageTimer - player->invincibiltyTimer >= 1.0f) {
				player->isPlayerInvincible = false;
				player->invincibiltyTimer = 0.0f;
				player->PLAYER_SPEED = speed;
			}
		}
	}
	for (EnemyBlue* enemy : enemiesBlue) {
		float timeFrameTime = player->invincibiltyTimer;
		float speed = player->invincibleSpeed;
		if (player_box.TestAABB(enemy->GetHitbox())) {

			if (!player->isPlayerInvincible) {
				player->takeDamage(1);
				player->isPlayerInvincible = true;
				speed = player->PLAYER_SPEED;
				player->PLAYER_SPEED = 0.0f;
				timeFrameTime = stageTimer;
			}
			player->invincibiltyTimer = timeFrameTime;
			player->invincibleSpeed = speed;
		}
		if (player->isPlayerInvincible) {
			if (stageTimer - player->invincibiltyTimer >= 1.0f) {
				player->isPlayerInvincible = false;
				player->invincibiltyTimer = 0.0f;
				player->PLAYER_SPEED = speed;
			}
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
	
	for (EnemyRed* enemyRed : enemiesRed)
	{
		delete enemyRed;
	}
	enemiesRed.clear();
	
	for (EnemyBlue* enemyBlue : enemiesBlue)
	{
		delete enemyBlue;
	}
	enemiesBlue.clear();
	
	for (auto* explosion : explosions) {
		delete explosion;
	}
	explosions.clear();
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
	//temporal approach
	DrawText(TextFormat("SCORE : %d", player->GetScore()), 150, 10, 10, LIGHTGRAY);
	DrawText(TextFormat("HEALTH : %d", player->GetHealth()), 300, 10, 10, LIGHTGRAY);

	if (startTimer)
	{
		int timeLeft = (int)ceil(200.0f - stageTimer);
		DrawText(TextFormat("Time: %03d", timeLeft), 10, 10, 10, LIGHTGRAY);
	}
}