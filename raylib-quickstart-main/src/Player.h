#pragma once
#include "Entity.h"
#include "TileMap.h"
#include <vector>
#include "Explosion.h"

//Representation model size: 16x16
#define PLAYER_FRAME_SIZE		16

//Logical model size: 12x28
#define PLAYER_PHYSICAL_WIDTH	6
#define PLAYER_PHYSICAL_HEIGHT	6

//Horizontal speed and vertical speed while falling down
#define PLAYER_SPEED			2

//Logic states
enum class State { IDLE, WALKING, DEAD };
enum class Look { RIGHT, LEFT, UP, DOWN };

//Rendering states
enum class PlayerAnim {
	IDLE_LEFT, IDLE_RIGHT, IDLE_UP,IDLE_DOWN,
	WALKING_LEFT, WALKING_RIGHT, WALKING_UP,WALKING_DOWN,
	NUM_ANIMATIONS
};

class Player: public Entity
{
public:
	Player(const Point& p, State s, Look view);
	~Player();
	
	AppStatus Initialise();
	void SetTileMap(TileMap* tilemap);

	void InitScore();
	void IncrScore(int n);
	int GetScore();

	void Update();
	void DrawDebug(const Color& col) const;
	void Release();
	Point GetPos() const { return pos; }
	void SetPos(const Point& p) { pos = p; }
	int GetHealth() const { return health; }
	void takeDamage(int damage);
	bool victory= false;

public:
	bool IsLookingRight() const;
	bool IsLookingLeft() const;
	bool IsLookingUp() const;
	bool IsLookingDown() const;

	//Player mechanics
	void MoveX();
	void MoveY();


	//Animation management
	void SetAnimation(int id);
	PlayerAnim GetAnimation();
	void Stop();
	void StartWalkingLeft();
	void StartWalkingRight();
	void StartWalkingUp();
	void StartWalkingDown();
	
	void ChangeAnimRight();
	void ChangeAnimLeft();
	void ChangeAnimUp();
	void ChangeAnimDown();

	
	//Ladder get in/out steps
	bool IsInFirstHalfTile() const;
	bool IsInSecondHalfTile() const;

	State state;
	Look look;

	TileMap *map;

	struct Bomb {
		Point position;
		float timer;
	};
	std::vector<Bomb> activeBombs;

	int score;
	int health = 3;
	int maxBombs = 1;
	float bombCooldown = 0.0f;
private:

	void StepsBrain();
	float stepsTimer = 1;

	Point direction = { 0, 0 };

};

