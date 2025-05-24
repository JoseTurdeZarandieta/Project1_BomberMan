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
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_RIGHT, { 0, 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_LEFT, { 0, n, n, n });

	
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_UP, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_UP, { 0, 3*n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_DOWN, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_DOWN, { 0,2*n, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT, ANIM_DELAY);
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT, { (float)i * n, 0, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_LEFT, ANIM_DELAY);
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT, { (float)i * n, n, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_UP, ANIM_DELAY);
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_UP, { (float)i * n, 3*n, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_DOWN, ANIM_DELAY);
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_DOWN, { (float)i * n, 2*n, n, n });

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
	int doorX;
	if (map->GetObjectAtPosition(box, &doorX) == Tile::ITEM_BOMB_UP) {
		maxBombs += 1;
	}
	if (IsKeyPressed(KEY_SPACE) && map->GetObjectAtPosition(box, &doorX) == Tile::DOOR)
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

//void Player::MoveX()
//{
//	
//	AABB box;
//	int prev_x = pos.x;
//
//
//	if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT) )
//	{ 
//		direction.x = -1;
//		if (state == State::IDLE) StartWalkingLeft();
//		else
//		{
//			if (IsLookingRight()) ChangeAnimLeft();
//		}
//
//		pos.x += -PLAYER_SPEED;
//
//		AABB box = GetHitbox();
//		if (map->TestCollisionWallLeft(box))
//		{
//			pos.x = prev_x;
//			//if (state == State::WALKING) direction.x = 0;
//			
//		}
//	}
//	else if (IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT))
//	{
//		direction.x = 1;
//		if (state == State::IDLE) StartWalkingRight();
//		else
//		{
//			if (IsLookingLeft()) ChangeAnimRight();
//		}
//
//		pos.x += PLAYER_SPEED;
//		AABB box = GetHitbox();
//		if (map->TestCollisionWallRight(box))
//		{
//			pos.x = prev_x;
//			//if (state == State::WALKING) direction.x = 0;
//		}
//	}
//	//else {
//	//	direction.x = 0;
//	//}
//	if (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT)) {
//		direction.x = 0;
//	}
//}
void Player::MoveX()
{
	AABB box;
	int prev_x = pos.x;

	if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT))
	{
		direction.x = -1;

		// Ensure animation matches direction
		if (state == State::IDLE) StartWalkingLeft();
		else if (!IsLookingLeft()) ChangeAnimLeft();

		pos.x += -PLAYER_SPEED;
		box = GetHitbox();
		if (map->TestCollisionWallLeft(box))
		{
			pos.x = prev_x;
		}
	}
	else if (IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT))
	{
		direction.x = 1;

		if (state == State::IDLE) StartWalkingRight();
		else if (!IsLookingRight()) ChangeAnimRight();

		pos.x += PLAYER_SPEED;
		box = GetHitbox();
		if (map->TestCollisionWallRight(box))
		{
			pos.x = prev_x;
		}
	}
	else {
		direction.x = 0;
	}
}
<<<<<<< Updated upstream
//check1
//void Player::MoveY()
//{
//	AABB box;
//	int prev_y = pos.y;
//
//
//	if (IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
//	{
//		pos.y -= PLAYER_SPEED;
//		direction.y = 1;
//		if (state == State::IDLE) StartWalkingUp();
//		else if (!IsLookingUp())
//			ChangeAnimUp();
//
//		box = GetHitbox();
//		if (map->TestCollisionWallUp(box))
//		{
//			pos.y = prev_y;
//			if (state == State::WALKING) direction.y = 0;
//		}
//	}
//	else if (IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_UP))
//	{
//		pos.y += PLAYER_SPEED;
//		direction.y = -1;
//		if (state == State::IDLE) StartWalkingDown();
//		else if (!IsLookingDown())
//			ChangeAnimDown();
//
//		box = GetHitbox();
//		if (map->TestCollisionGround(box, &pos.y))
//		{
//			pos.y = prev_y;
//			if (state == State::WALKING) direction.y = 0;
//		}
//	}
//	else {
//		direction.y = 0;
//	}
//}
=======

>>>>>>> Stashed changes
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
		if (map->TestCollisionWallUp(box))
		{
			pos.y = prev_y;
		}
	}
	else if (IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_UP))
	{
		direction.y = 1;

		if (state == State::IDLE) StartWalkingDown();
		else if (!IsLookingDown()) ChangeAnimDown();

		pos.y += PLAYER_SPEED;
		box = GetHitbox();
		if (map->TestCollisionGround(box, &pos.y))
		{
			pos.y = prev_y;
		}
	}
	else {
		direction.y = 0;
	}
}
//check2
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