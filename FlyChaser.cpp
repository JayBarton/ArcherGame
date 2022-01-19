#include "FlyChaser.h"

const float FlyChaser::chaseRadius = 192.0f;
const float FlyChaser::explodeRadius = 48.0f;
const float FlyChaser::speed = 1.5f;

FlyChaser::FlyChaser(glm::vec2 p, int facing, Sprite & player, Mix_Chunk* sound) :  player(player)
{
    init(p, facing);

    addCollider(glm::vec4(0, 8, 32, 24), Collider::NORMAL);

    startPosition = p;
    spawnArea.p = startPosition;
    startFacing = facing;

    bobPosition = startPosition;
    bobPosition.y -= 16.0f;

    sprite.startingFrame = LEFT_START_FRAME;

    explodeSound = sound;

}

void FlyChaser::update(float delta)
{
    if(!exploding)
    {
        glm::vec2 toPlayer = player.getHitBoxCenter() - sprite.getHitBoxCenter();
        float distanceToPlayer = glm::length(toPlayer);

        if(distanceToPlayer <= explodeRadius)
        {
            beginExplosion();
        }
        else
        {
            if(toPlayer.x > 0)
            {
                sprite.facing = 1;
            }
            else
            {
                sprite.facing = -1;
            }
            if (!chasing)
            {
                if (distanceToPlayer <= chaseRadius)
                {
                    chasing = true;
                    if(sprite.facing > 0)
                    {
                        sprite.startingFrame = RIGHT_CHASE_START_FRAME;
                        sprite.currentFrame = RIGHT_CHASE_START_FRAME;
                    }
                    else
                    {
                        sprite.startingFrame = LEFT_CHASE_START_FRAME;
                        sprite.currentFrame = LEFT_CHASE_START_FRAME;
                    }
                }
                else
                {
                    if(sprite.facing > 0)
                    {
                        sprite.startingFrame = RIGHT_START_FRAME;
                        if(sprite.currentFrame < RIGHT_START_FRAME)
                        {
                            sprite.currentFrame = RIGHT_START_FRAME;
                        }
                    }
                    else
                    {
                        sprite.startingFrame = LEFT_START_FRAME;
                    }
                    /*if(bob)
                    {
                        glm::vec2 direction = glm::vec2(0.0f, -1.0f);
                        sprite.Move(direction * 0.5f);
                        if(sprite.getPosition().y < bobPosition.y)
                        {
                            bob = false;
                        }
                    }
                    else
                    {
                        glm::vec2 direction = glm::vec2(0.0f, 1.0f);

                        sprite.Move(direction * 0.5f);
                        if(sprite.getPosition().y > startPosition.y)
                        {
                            bob = true;
                        }
                    }*/
                }
            }
            if (chasing)
            {
                if(sprite.facing > 0)
                {
                    sprite.startingFrame = RIGHT_CHASE_START_FRAME;
                }
                else
                {
                    sprite.startingFrame = LEFT_CHASE_START_FRAME;
                }
                glm::vec2 direction = glm::normalize(toPlayer);
                sprite.Move(direction * speed);
            }
        }
        sprite.playAnimation(delta, 4, true);
    }
    else
    {
        bool animationFinished = sprite.playAnimationOnce(delta, 3, true);
        if(animationFinished)
        {
            explode();
        }
    }
}

void FlyChaser::beginExplosion()
{
    exploding = true;
    if(sprite.facing < 0)
    {
        sprite.startingFrame = 16;
        sprite.currentFrame = 16;
    }

    else
    {
        sprite.startingFrame = 19;
        sprite.currentFrame = 19;
    }
}


void FlyChaser::explode()
{
    exploding = false;
    sprite.active = false;
    //Explosion animation plays here...
    glm::vec2 center = sprite.getHitBoxCenter();

    float explosionSize = 40;

    glm::vec2 newSize(explosionSize * 2, explosionSize * 2);

    sprite.setHitBox(glm::vec4(0, 0, newSize.x, newSize.y));
    sprite.setSize(newSize);
    sprite.SetPosition(center - explosionSize);
    sprite.startingFrame = 22;
    sprite.currentFrame = 22;

    //figure this out
    //colliders[0].dimensions = glm::vec4(0, 0, hb.z *2, hb.w *2);
    colliders.pop_back();
    addCollider(glm::vec4(0, 0, newSize.x, newSize.y), Collider::DAMAGE);
    health.takeDamage(1);

    Mix_PlayChannel(-1, explodeSound, 0);
}

void FlyChaser::takeHit(int damage)
{
    //  explode();
    beginExplosion();
}

void FlyChaser::init(glm::vec2 p, int facing)
{
    sprite.active = true;
    sprite.SetPosition(p);

    sprite.setHitBox(glm::vec4(0, 8, 32, 24));

    sprite.setSize(glm::vec2(32, 32));
    sprite.facing = facing;

    health = Health(1);
    health.special = true;

    explodingTimer = 0.0f;

    chasing = false;
    exploding = false;
}

void FlyChaser::handleDeath(float deltaTime)
{
    bool finished = sprite.playAnimationOnce(deltaTime, 4, false);
    if(finished)
    {
        health.dead = true;
    }
}

void FlyChaser::reset()
{
    init(startPosition, startFacing);
    colliders.pop_back();
    addCollider(glm::vec4(0, 0, 32, 32), Collider::NORMAL);
    sprite.startingFrame = LEFT_START_FRAME;
    sprite.currentFrame = sprite.startingFrame;

}

FlyChaser::~FlyChaser()
{
}
