#include "AudioManager.h"
#include "Player.h"
#include "Sprite.h"
#include "TileMap.h"
#include "Globals.h"
#include <raymath.h>

Player::Player(const Point& p, State s, Look view) :
	Entity(p, PLAYER_PHYSICAL_WIDTH, PLAYER_PHYSICAL_HEIGHT, PLAYER_FRAME_SIZE, PLAYER_FRAME_SIZE)
{
	state = s;
	look = view;
	map = nullptr;
	score = 0;
	victory = false;
}
Player::~Player()
{
}
AppStatus Player::Initialise()
{
	int i;
	const int n = PLAYER_FRAME_SIZE;

	ResourceManager& data = ResourceManager::Instance();
	if (data.LoadTexture(Resource::IMG_PLAYER, "resources/Sprites/General.png") != AppStatus::OK)
	{
		return AppStatus::ERROR;
	}

	render = new Sprite(data.GetTexture(Resource::IMG_PLAYER));
	if (render == nullptr)
	{
		LOG("Failed to allocate memory for player sprite");
		return AppStatus::ERROR;
	}

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetNumberAnimations((int)PlayerAnim::NUM_ANIMATIONS);

	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_RIGHT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_RIGHT, { n, n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_LEFT, { n, 0, n, n });

	
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_UP, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_UP, { 4*n, n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_DOWN, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_DOWN, { 4*n,0, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT, ANIM_DELAY);
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT, { (float)i * n, n, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_LEFT, ANIM_DELAY);
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT, { (float)i * n, 0, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_UP, ANIM_DELAY);
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_UP, { (float)(i+3) * n, n, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_DOWN, ANIM_DELAY);
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_DOWN, { (float)(i+3) * n, 0, n, n });

	sprite->SetAnimation((int)PlayerAnim::IDLE_RIGHT);

	return AppStatus::OK;;

}
void Player::InitScore()
{
	score = 0;
}
void Player::IncrScore(int n)
{
	score += n;
}
int Player::GetScore()
{
	return score;
}
void Player::SetTileMap(TileMap* tilemap)
{
	map = tilemap;
}
bool Player::IsLookingRight() const
{
	return look == Look::RIGHT;
}
bool Player::IsLookingLeft() const
{
	return look == Look::LEFT;
}
bool Player::IsLookingUp() const
{
	return look == Look::UP;
}
bool Player::IsLookingDown() const
{
	return look == Look::DOWN;
}

bool Player::IsInFirstHalfTile() const
{
	return pos.y % TILE_SIZE < TILE_SIZE / 2;
}
bool Player::IsInSecondHalfTile() const
{
	return pos.y % TILE_SIZE >= TILE_SIZE/2;
}
void Player::SetAnimation(int id)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetAnimation(id);
}
PlayerAnim Player::GetAnimation()
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	return (PlayerAnim)sprite->GetAnimation();
}
void Player::Stop()
{
	dir = { 0,0 };
	state = State::IDLE;
	if (IsLookingRight())	SetAnimation((int)PlayerAnim::IDLE_RIGHT);
	else if(IsLookingLeft())		 			SetAnimation((int)PlayerAnim::IDLE_LEFT);
	else if(IsLookingDown())		 			SetAnimation((int)PlayerAnim::IDLE_DOWN);
	else 		SetAnimation((int)PlayerAnim::IDLE_UP);

}
void Player::StartWalkingLeft()
{
	state = State::WALKING;
	look = Look::LEFT;
	SetAnimation((int)PlayerAnim::WALKING_LEFT);
	AudioManager::Instance().PlaySoundByName("HorizontalSteps");
}
void Player::StartWalkingRight()
{
	state = State::WALKING;
	look = Look::RIGHT;
	SetAnimation((int)PlayerAnim::WALKING_RIGHT);
	AudioManager::Instance().PlaySoundByName("HorizontalSteps");
}
void Player::StartWalkingUp()
{
	state = State::WALKING;
	look = Look::UP;
	SetAnimation((int)PlayerAnim::WALKING_UP);
	AudioManager::Instance().PlaySoundByName("VerticalSteps");
	LOG("Started walking UP");
}
void Player::StartWalkingDown()
{
	state = State::WALKING;
	look = Look::DOWN;
	SetAnimation((int)PlayerAnim::WALKING_DOWN);
	AudioManager::Instance().PlaySoundByName("VerticalSteps");
}

void Player::ChangeAnimRight()
{
	look = Look::RIGHT;
	switch (state)
	{
		case State::IDLE:	 SetAnimation((int)PlayerAnim::IDLE_RIGHT);    break; 
		case State::WALKING: SetAnimation((int)PlayerAnim::WALKING_RIGHT); break;
	}
}
void Player::ChangeAnimLeft()
{
	look = Look::LEFT;
	switch (state)
	{
		case State::IDLE:	 SetAnimation((int)PlayerAnim::IDLE_LEFT);    break;
		case State::WALKING: SetAnimation((int)PlayerAnim::WALKING_LEFT); break;
	}
}
void Player::ChangeAnimUp()
{
	look = Look::UP;
	switch (state)
	{
		case State::IDLE:	 SetAnimation((int)PlayerAnim::IDLE_UP);    break;
		case State::WALKING: SetAnimation((int)PlayerAnim::WALKING_UP);	break;
		
	}
}void Player::ChangeAnimDown()
{
	look = Look::DOWN;
	switch (state)
	{
		case State::IDLE:	 SetAnimation((int)PlayerAnim::IDLE_DOWN);    break;
		case State::WALKING: SetAnimation((int)PlayerAnim::WALKING_DOWN); break;
		
	}
}

void Player::Update()
{

	MoveY();
	MoveX();
	StepsBrain();
	stepsTimer += GetFrameTime();

	if (direction.x == 0 && direction.y == 0) {
		if (state == State::WALKING) Stop();
	}


	AABB box = GetHitbox();
	int objectX;

	int tileX = (box.pos.x + box.width / 2) / TILE_SIZE;
	int tileY = (box.pos.y + box.height - 1) / TILE_SIZE;

	if (tileX >= 0 && tileX < map->width && tileY >= 0 && tileY < map->height) {
		
		int temporarytile = tileY * map->width + tileX;

		if (map->GetObjectAtPosition(box, &objectX) == Tile::ITEM_BOMB_UP) {
			maxBombs += 1;
			map->map[temporarytile] = Tile::AIR;
		}
		if (map->GetObjectAtPosition(box, &objectX) == Tile::ITEM_FIRE_UP) {
			fire_range += 1;
			map->map[temporarytile] = Tile::AIR;
		}
		if (map->GetObjectAtPosition(box, &objectX) == Tile::ITEM_SPEED_UP) {
			PLAYER_SPEED += 1;
			map->map[temporarytile] = Tile::AIR;
		}
		if (map->GetObjectAtPosition(box, &objectX) == Tile::ITEM_REMOTE_CONTROL) {
			remotecontrol = true;
			map->map[temporarytile] = Tile::AIR;
		}
	}

	if (IsKeyPressed(KEY_SPACE) && map->GetObjectAtPosition(box, &objectX) == Tile::DOOR)
	{
		victory = true;
	}

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->Update();

}

void Player::StepsBrain() {
	if (stepsTimer < 0.3) {
		return;
	}
	
	if (direction.x != 0) {
		AudioManager::Instance().PlaySoundByName("HorizontalSteps");
	}
	else if (direction.y != 0) {
		AudioManager::Instance().PlaySoundByName("VerticalSteps");
	}
	stepsTimer = 0;
}

void Player::MoveX()
{
	AABB box;
	int prev_x = pos.x;

	if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT))
	{
		direction.x = -1;

		if (state == State::IDLE) StartWalkingLeft();
		else if (!IsLookingLeft()) ChangeAnimLeft();

		pos.x += -PLAYER_SPEED;
		box = GetHitbox();
		bool hitLeft = map->TestCollisionWallLeft(box);
		if (hitLeft && bombCooldown>0.0f)
		{
			int collX = box.pos.x / TILE_SIZE;
			int collY1 = box.pos.y / TILE_SIZE;
			int collY2 = (box.pos.y + box.height - 1) / TILE_SIZE;
			bool isThatABomb = true;
			for (int y = collY1; y <= collY2; ++y) {
				if (map->GetTileIndex(collX, y) != Tile::BOMB) {
					isThatABomb = false;
					break;
				}
			}
			if (isThatABomb) {
				hitLeft = false;
			}
		}
		if (hitLeft) {
			pos.x = prev_x;
			if (state == State::WALKING) {
				direction.x = 0;
			}
		}
	}
	else if (IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT))
	{
		direction.x = 1;

		if (state == State::IDLE) StartWalkingRight();
		else if (!IsLookingRight()) ChangeAnimRight();

		pos.x += PLAYER_SPEED;
		box = GetHitbox();

		int safeX = pos.x;
		bool hitRight = map->TestCollisionWallRight(box);
		
		if (hitRight && bombCooldown > 0.0f)
		{
			int collX = (box.pos.x) / TILE_SIZE;
			int collY1 = box.pos.y / TILE_SIZE;
			int collY2 = (box.pos.y + box.height - 1) / TILE_SIZE;

			bool isThatABomb = true;
			for (int y = collY1; y <= collY2; ++y) {
				if (map->GetTileIndex(collX, y) != Tile::BOMB) {
					isThatABomb = false;
					break;
				}
			}
			if (!isThatABomb) {
				hitRight = map->TestCollisionWallRight(box);
			}
		}
		else {
			hitRight = hitRight = map->TestCollisionWallRight(box);
		}
		if (hitRight) {
			pos.x = prev_x;
			if (state == State::WALKING) {
				direction.x = 0;
			}
		}
		else {
			pos.x = safeX;
		}
	}
	else {
		direction.x = 0;
	}
}

void Player::MoveY()
{
	AABB box;
	int prev_y = pos.y;

	if (IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
	{
		direction.y = -1;

		if (state == State::IDLE) StartWalkingUp();
		else if (!IsLookingUp()) ChangeAnimUp();

		pos.y -= PLAYER_SPEED;
		box = GetHitbox();
		
		bool hitUp = map->TestCollisionWallUp(box);
		if (hitUp && bombCooldown > 0.0f)
		{
			int collY = box.pos.y / TILE_SIZE;
			int collX1 = box.pos.x / TILE_SIZE;
			int collX2 = (box.pos.x + box.width - 1) / TILE_SIZE;
			bool isThatABomb = true;
			for (int x = collX1; x <= collX2; ++x) {
				if (map->GetTileIndex(x, collY) != Tile::BOMB) {
					isThatABomb = false;
					break;
				}
			}
			if (isThatABomb) {
				hitUp = false;
			}
		}
		if (hitUp) {
			pos.y = prev_y;
			if (state == State::WALKING) {
				direction.y = 0;
			}
		}
	}
	else if (IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_UP))
	{
		direction.y = 1;

		if (state == State::IDLE) StartWalkingDown();
		else if (!IsLookingDown()) ChangeAnimDown();

		pos.y += PLAYER_SPEED;
		box = GetHitbox();

		int safeY = pos.y;
		bool hitDown = map->TestCollisionGround(box, &safeY);;
		if (hitDown && bombCooldown > 0.0f)
		{
			int collY = (box.pos.y + box.height - 1) / TILE_SIZE;
			int collX1 = box.pos.x / TILE_SIZE;
			int collX2 = (box.pos.x + box.width - 1) / TILE_SIZE;

			bool isThatABomb = true;
			for (int x = collX1; x <= collX2; ++x) {
				if (map->GetTileIndex(x, collY) != Tile::BOMB) {
					isThatABomb = false;
					break;
				}
			}
			if (!isThatABomb) {
				hitDown = map->TestCollisionGround(box, &safeY);
			}
		}
		else {
			hitDown = map->TestCollisionGround(box, &safeY);
		}
		if (hitDown) {
			pos.y = prev_y;
			if (state == State::WALKING) {
				direction.y = 0;
			}
		}
		else {
			pos.y = safeY;
		}
	}
	else {
		direction.y = 0;
	}
}

void Player::DrawDebug(const Color& col) const
{	
	Entity::DrawHitbox(pos.x, pos.y, width, height, col);
	
	DrawText(TextFormat("Position: (%d,%d)\nSize: %dx%d\nFrame: %dx%d", pos.x, pos.y, width, height, frame_width, frame_height), 18*16, 0, 8, LIGHTGRAY);
	DrawPixel(pos.x, pos.y, WHITE);
}
void Player::Release()
{
	ResourceManager& data = ResourceManager::Instance();
	data.ReleaseTexture(Resource::IMG_PLAYER);

	render->Release();
}
void Player::takeDamage(int damage)
{
	if (health > 0)
	{
		health -= damage;
		if (health <= 0)
		{
			state = State::DEAD;
			victory = false;
		}
	}

}