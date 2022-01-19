#include "WalkBehaviour.h"

WalkBehaviour::WalkBehaviour()
{
}

WalkBehaviour::WalkBehaviour(float speed, GameObject *actor) : walkSpeed(speed), actor(actor)
{
    sprite = &actor->sprite;
    physics = actor->physics;
}


void WalkBehaviour::init(GameObject *actor)
{
    this->actor = actor;
    sprite = &actor->sprite;
    physics = actor->physics;
}

void WalkBehaviour::update(float delta)
{
    if(!actor->health.hit)
    {
        input();
        hitDirection = false;
    }
    else
    {
        hitDirection = true;
    }
    physics->update(*sprite, delta);

    if(physics->state == Physics::ON_GROUND && !actor->health.damaged)
    {
        float xPositionThisFrame = sprite->getPosition().x;
        if(xPositionLastFrame == xPositionThisFrame)
        {
            samePositionCount++;
            if(samePositionCount >= 3)
            {
                stuck = true; //All this is to handle the case in which a walker can't move. Should only happen in level 1 map 4
            }
        }
        else
        {
            samePositionCount= 0;
        }
        xPositionLastFrame = xPositionThisFrame;
    }

    if(!stuck)
    {
        checkTurn();
    }
}

void WalkBehaviour::input()
{
    if (physics->state == Physics::ON_GROUND)
    {
        physics->moveSpeed.x = sprite->facing;
        physics->currentSpeed = walkSpeed;
    }
}

void WalkBehaviour::checkTurn()
{
    if (physics->state == Physics::ON_GROUND)
    {
        if (physics->leftRightSensor)
        {
            turnAround();
        }
        else if ((physics->moveSpeed.x < 0 && !physics->bottomLeftSensor) ||
                 (physics->moveSpeed.x > 0 && !physics->bottomRightSensor))
        {
            turnAround();
        }
    }
}

void WalkBehaviour::turnAround()
{
    //Should only turn around if walked into the wall, not if a player arrow knocked them into it
    if(!hitDirection)
    {
        sprite->facing *= -1;
    }
    physics->currentSpeed = walkSpeed;
}


WalkBehaviour::~WalkBehaviour()
{
}
