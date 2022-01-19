#include "Physics.h"
#include "TileManager.h"
#include "SATCollision.h"

Physics::Physics()
{
    currentSpeed = 0;
    state = State::IN_AIR;
}

Physics::Physics(float fallAcceleration, float maxFallSpeed)
{
    this->fallAcceleration= fallAcceleration;
    this->maxFallSpeed = maxFallSpeed;
}


Physics::~Physics()
{
}

void Physics::update(Sprite & sprite, float delta)
{
    int samples = 1;

    bool wasInAir = false;
    if(state == State::IN_AIR)
    {
        applyGravity(delta);
        samples = 4;
        wasInAir = true;
    }
    //for the player dash
    else if(currentSpeed >= 6)
    {
        samples = 4;
    }

    for(int i = 0; i < samples; i ++)
    {
        glm::vec2 movement = glm::vec2(moveSpeed.x * currentSpeed, moveSpeed.y) / float(samples);
        sprite.Move(movement);
        checkTileBounds(sprite);
        //break out of sampling on landing or on hitting a wall
        if(topSensor)
        {
            break;
        }
        if(wasInAir && state == State::ON_GROUND)
        {
            break;
        }
        if(state == State::ON_GROUND && leftRightSensor)
        {
            break;
        }
    }

    checkLevelBounds(sprite);
}

void Physics::push(glm::vec2 velocity, float xSpeed)
{
    moveSpeed = velocity;
    currentSpeed = xSpeed;

    if (velocity.y < 0)
    {
        state = State::IN_AIR;
    }
}

void Physics::checkTileBounds(Sprite & sprite)
{
    glm::vec4 hb = sprite.getHitBox();

    leftRightSensor = false;
    topSensor = false;
    bottomLeftSensor = false;
    bottomRightSensor = false;

    checkHorizontalCollisions(sprite, hb);

    checkVerticalCollisions(sprite, hb);
}

void Physics::checkHorizontalCollisions(Sprite& sprite, glm::vec4& hb)
{
    hb = sprite.getHitBox();
    if (moveSpeed.x != 0)
    {
        if(state == ON_GROUND)
        {
            //Check the point that is in front of the actor and at half their height
            glm::vec2 tilePoint = glm::vec2(hb.x + hb.z * 0.5f + (moveSpeed.x * hb.z * 0.5f), hb.y + hb.w * 0.5f);
            leftRightTileCheck(sprite, tilePoint, hb);
        }
        else
        {
            //While in the air, want to check the corners of the sprite, this prevents them "climbing" up/below blocks
            glm::vec2 tilePoint = glm::vec2(hb.x + hb.z * 0.5f + (moveSpeed.x * hb.z * 0.5f), hb.y );
            leftRightTileCheck(sprite, tilePoint, hb);
            //If the first corner was blocked, no need to check the second
            if(!leftRightSensor)
            {
                tilePoint = glm::vec2(hb.x + hb.z * 0.5f + (moveSpeed.x * hb.z * 0.5f), hb.y + hb.w  );
                leftRightTileCheck(sprite, tilePoint, hb);
            }
        }
        hb = sprite.getHitBox();
    }
    else
    {
        //this is primarily to check collisions made while parented, since the x speed is technically 0
        //Potentially both sides need to be checked, since I don't know the parent's direction
        glm::vec2 tilePoint = glm::vec2(hb.x + hb.z * 0.5f + (1 * hb.z * 0.5f), hb.y + hb.w * 0.5f);
        leftRightTileCheck(sprite, tilePoint, hb);

        //Can't collide with both sides, if first side was blocked no need to check the second.
        if(!leftRightSensor)
        {
            tilePoint = glm::vec2(hb.x + hb.z * 0.5f + (-1 * hb.z * 0.5f), hb.y + hb.w * 0.5f);
            leftRightTileCheck(sprite, tilePoint, hb);
        }

        hb = sprite.getHitBox();
    }
}

void Physics::checkVerticalCollisions(Sprite& sprite, glm::vec4& hb)
{
    checkBottom(sprite, hb);

    checkTop(sprite, hb);
}
void Physics::checkBottom(Sprite& sprite, glm::vec4& hb)
{
    int xOffset = 4;
    glm::vec2 bottomRight = glm::vec2(hb.x + hb.z - xOffset, hb.y + hb.w);
    glm::vec2 bottomLeft = glm::vec2(hb.x + xOffset, hb.y + hb.w);

    bottomLeftSensor = !TileManager::tileManager.airPassable(bottomLeft.x, bottomLeft.y);
    bottomRightSensor = !TileManager::tileManager.airPassable(bottomRight.x, bottomRight.y);

    if (!bottomRightSensor && !bottomLeftSensor)
    {
        if(sprite.parent == nullptr)
        {
            state = State::IN_AIR;
        }
    }
    else
    {
        if (moveSpeed.y > 0 || sprite.parent)
        {
            glm::vec2 bottomPoint;

            if(bottomRightSensor)
            {
                bottomPoint = bottomRight;
            }
            else
            {
                bottomPoint = bottomLeft;
            }

            SimpleShape tileBox = TileManager::tileManager.getTileBox(bottomPoint.x, bottomPoint.y);

            SimpleShape pBox;
            pBox = newSquare(sprite.getHitBoxCenter(), hb.z, hb.w);

            glm::vec2 push(0.0f);

            if(bottomPoint.y < tileBox.getPosition().y && satCollision(pBox, tileBox, push.x, push.y) && push.y < 0)
            {
                sprite.Move(push);
                hb = sprite.getHitBox();
                land();
                sprite.setParent(nullptr);
            }
        }
    }
}

void Physics::checkTop(Sprite& sprite, glm::vec4& hb)
{
    if (moveSpeed.y < 0)
    {
        glm::vec2 topLeft = glm::vec2(hb.x + 4, hb.y - 2);
        glm::vec2 topRight = glm::vec2(hb.x + hb.z - 4, hb.y - 2);

        glm::vec2 topPoint;

        if(!TileManager::tileManager.tileIsPassable(topLeft.x, topLeft.y))
        {
            topSensor = true;
            topPoint = topLeft;
        }
        else if(!TileManager::tileManager.tileIsPassable(topRight.x, topRight.y))
        {
            topSensor = true;
            topPoint = topRight;
        }

        if (topSensor)
        {
            topPoint = glm::vec2(sprite.getHitBoxCenter().x, hb.y - 2);

            SimpleShape pBox;
            pBox = newSquare(sprite.getHitBoxCenter(), hb.z, hb.w);

            SimpleShape tileBox = TileManager::tileManager.getTileBox(topPoint.x, topPoint.y);

            glm::vec2 push(0);
            //might want to verify that the y push is downwards
            //I'm thinking that since the x push is calculated first, there should never be an instance
            //where we get down her and y won't be downwards
            if(satCollision(pBox, tileBox, push.x, push.y))
            {
                moveSpeed.y = 0;
                sprite.Move(push);
                hb = sprite.getHitBox();
            }
        }
    }
}


void Physics::leftRightTileCheck(Sprite& sprite, glm::vec2 tilePoint, const glm::vec4 &hb)
{
    leftRightSensor = !TileManager::tileManager.tileIsPassable(tilePoint.x, tilePoint.y);
    if (leftRightSensor)
    {
        SimpleShape pBox;
        pBox = newSquare(sprite.getHitBoxCenter(), hb.z, hb.w);
        SimpleShape tileBox = TileManager::tileManager.getTileBox(tilePoint.x, tilePoint.y);
        //	int type = TileManager::tileManager.getType(rightPoint.x, rightPoint.y);

        glm::vec2 push(0);
        satCollision(pBox, tileBox, push.x, push.y);

        if(abs(push.x) > 0)
        {
            sprite.Move(push);
            moveSpeed.x = 0;
        }
        else
        {
            leftRightSensor = false;
        }
    }
}


void Physics::checkLevelBounds(Sprite& sprite)
{
    glm::vec2 position = sprite.getPosition();
    glm::vec4 hitBox = sprite.getHitBox();
    if (hitBox.x < 0)
    {
        sprite.SetPosition(glm::vec2(0.0f, position.y));
        moveSpeed.x = 0;
        leftRightSensor = true;
    }
    else if (hitBox.x + hitBox.z > TileManager::tileManager.levelWidth)
    {
        sprite.SetPosition(glm::vec2(TileManager::tileManager.levelWidth - hitBox.z, position.y));
        moveSpeed.x = 0;
        leftRightSensor = true;
    }

    if(hitBox.y > TileManager::tileManager.levelHeight)
    {
        sprite.active = false;
    }
}


void Physics::land()
{
    moveSpeed.y = 0.0f;
    state = State::ON_GROUND;
}

void Physics::applyGravity(float delta)
{
    moveSpeed.y += fallAcceleration;
    if (moveSpeed.y > maxFallSpeed)
    {
        moveSpeed.y = maxFallSpeed;
    }
}


void Physics::reset()
{
    currentSpeed = 0.0f;
    state = State::IN_AIR;

    moveSpeed = glm::vec2(0);
}
