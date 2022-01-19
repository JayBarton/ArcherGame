#pragma once
#include "Sprite.h"
#include "Physics.h"
#include "GameObject.h"
class WalkBehaviour
{
public:
	WalkBehaviour();
	WalkBehaviour(float speed, GameObject *actor);

	void init(GameObject *actor);

	float walkSpeed = 1.5f;

	//These three variables are to prevent the walker from turning infinitely if they are stuck
	//This only happens in one place, but it's an important place to teach the player how to kill the walkers
    float xPositionLastFrame = 0;
    int samePositionCount = 3;
    bool stuck = false;

    //Used to prevent the walker from turning if an arrow pushed them into a wall
    bool hitDirection = false;

	void update(float delta);

	void input();
	void checkTurn();
	void turnAround();

    Physics* physics;

	~WalkBehaviour();

private:
    GameObject *actor;
	Sprite* sprite;

};

