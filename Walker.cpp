#include "Walker.h"

Walker::Walker(glm::vec2 p, int facing)
{
    startPosition = p;
    startFacing = facing;
    spawnArea.p = startPosition;

    //Allow some extra leeway before this enemy despawns
    despawnArea.x = 192;

    sprite.setHitBox(glm::vec4(0, 0, 32, 32));

    sprite.setSize(glm::vec2(32, 32));

    physics = new Physics();

    walk = WalkBehaviour(walkSpeed, this);

    addCollider(glm::vec4(0, 2, 1, 28), Collider::ARMOR);
    addCollider(glm::vec4(31, 2, 1, 28), Collider::ARMOR);
    addCollider(glm::vec4(0, 31, 32, 1), Collider::ARMOR);
    addCollider(glm::vec4(0, 0, 32, 32), Collider::WALKABLE);
    addCollider(glm::vec4(4, 4, 24, 22), Collider::NORMAL);

  //  addCollider(glm::vec4(-2, 4, 32, 32), Collider::ARMOR);
   // addCollider(glm::vec4(4, 4, 32, 32), Collider::ARMOR);


    init(p, facing);
}

void Walker::init(glm::vec2 p, int facing)
{
    sprite.active = true;
    sprite.SetPosition(p);
    sprite.currentFrame = 0;

    sprite.facing = facing;
    health = Health(3);

    physics->reset();
}

void Walker::update(float delta)
{
    walk.update(delta);

    if(!walk.stuck && !knockedBack)
    {
        handleAnimation(delta);
    }
    if(knockedBack)
    {
        if(physics->state == physics->ON_GROUND)
        {
            knockedBack = false;
            if(walk.stuck)
            {
                sprite.currentFrame = 0;
            }
        }
    }

    health.updateDamaged(delta);
}

void Walker::handleAnimation(float delta)
{
    if(sprite.facing > 0)
    {
        sprite.startingFrame = START_RIGHT_FRAME;
        if (sprite.currentFrame >= START_LEFT_FRAME)
        {
            sprite.currentFrame = sprite.startingFrame;
        }
    }
    else
    {
        sprite.startingFrame = START_LEFT_FRAME;
        if (sprite.currentFrame < sprite.startingFrame || sprite.currentFrame > START_LEFT_FRAME + FRAMES_PER_ANIMATION)
        {
            sprite.currentFrame = sprite.startingFrame;
        }
    }
    sprite.playAnimation(delta, FRAMES_PER_ANIMATION, true);
}

void Walker::takeHit(int damage)
{
    GameObject::takeHit(damage);
    if(damage > 0)
    {
        knockedBack = true;
        sprite.currentFrame = 8;
        sprite.startingFrame = 8;
    }
}


void Walker::reset()
{
    init(startPosition, startFacing);
}

Walker::~Walker()
{
    delete physics;
}

