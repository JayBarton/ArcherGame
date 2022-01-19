#pragma once
#include "Sprite.h"
#include "Physics.h"
#include "WalkBehaviour.h"

#include "GameObject.h"
class Walker : public GameObject
{
public:
	Walker(glm::vec2 p, int facing);

	WalkBehaviour walk;

	glm::vec2 startPosition;
	int startFacing;

	void update(float delta);

	void init(glm::vec2 p, int facing);

	void reset();

    void takeHit(int damage);

	~Walker();

private:

    const static int START_RIGHT_FRAME = 0;
    const static int START_LEFT_FRAME = 4;
    const static int FRAMES_PER_ANIMATION = 4;
    const static int HURT_FRAME = 8;

    bool knockedBack = false; //duplicating from charge enemy

	float walkSpeed = 1.0f;

	void handleAnimation(float delta);

	//void input();
	//void checkTurn();
	//void turnAround();
};

