#pragma once
#include "Sprite.h"
#include "GameObject.h"

#include "SDL_mixer.h"
class FlyChaser : public GameObject
{
public:

	FlyChaser(glm::vec2 p, int facing, Sprite &player, Mix_Chunk* sound);

	static const float chaseRadius; //how close the player must be for the enemy to pursue
	static const float explodeRadius; //How close to the player before exploding
	static const float speed; //how fast this moves when chasing

    static const int LEFT_START_FRAME = 0;
	static const int RIGHT_START_FRAME = 4;

	static const int LEFT_CHASE_START_FRAME = 8;
	static const int RIGHT_CHASE_START_FRAME = 12;

	glm::vec2 startPosition;

	//Test animation bob
	glm::vec2 bobPosition;
	bool bob = true;

	int startFacing;

	Sprite &player;

	bool chasing = false;
	bool exploding = false;

	//How long the explosion effect lasts
	float explodingTimer = 0.0f;
	float explodingTime = 0.4f;

	Mix_Chunk* explodeSound;

	void update(float delta);
	void beginExplosion();
	void explode();
    void takeHit(int damage);

    void handleDeath(float deltaTime);

    void init(glm::vec2 p, int facing);
    void reset();

	~FlyChaser();
};

