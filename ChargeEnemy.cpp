#include "ChargeEnemy.h"
#include "TileManager.h"
#include "SATCollision.h"

ChargeEnemy::ChargeEnemy(glm::vec2 p, int facing, bool armor, Sprite & player, Mix_Chunk* sound) : player(player)
{
    startPosition = p;
    startFacing = facing;
    spawnArea.p = startPosition;

    //Allow some extra leeway before this enemy despawns
    despawnArea.x = 192;

    sprite.setHitBox(glm::vec4(0, 0, 32, 32));

    sprite.setSize(glm::vec2(32, 32));

    physics = new Physics();

    this->armor = armor;
    if(armor)
    {
        addCollider(glm::vec4(0, 0, 30, 32), Collider::ARMOR);
    }
    addCollider(glm::vec4(0, 0, 32, 32), 0);

    noticePlayerSound = sound;

    init(p, facing);
}

void ChargeEnemy::init(glm::vec2 p, int facing)
{
    sprite.active = true;
    sprite.SetPosition(p);

    sprite.facing = facing;
    physics->reset();

    charging = false;
    health = Health(3);

    if(armor)
    {
        if(sprite.facing < 0)
        {
            colliders[0].dimensions.x = 0;
        }
        else
        {
            colliders[0].dimensions.x = 2;
        }
    }
}

void ChargeEnemy::reset()
{
    init(startPosition, startFacing);
}


void ChargeEnemy::update(float delta)
{
    //Don't like the double check on this condition, but input has to be before physics, and physics has to be before check turn
    if(!health.hit)
    {
        input();
    }

    physics->update(sprite, delta);

    if(!knockedBack)
    {
        checkTurn();
        handleAnimation(delta);
    }
    else
    {
        sprite.currentFrame = HURT_FRAME;
        if(physics->state == physics->ON_GROUND)
        {
            knockedBack = false;
        }
    }

    health.updateDamaged(delta);
}

void ChargeEnemy::handleAnimation(float delta)
{
    int startFrameRight = START_FRAME_WALK_RIGHT;
    int startFrameLeft = START_FRAME_WALK_LEFT;

    if(charging)
    {
        startFrameRight = START_FRAME_RUN_RIGHT;
        startFrameLeft = START_FRAME_RUN_LEFT;
    }
    if(sprite.facing > 0)
    {
        sprite.startingFrame = startFrameRight;
        if (sprite.currentFrame < sprite.startingFrame || sprite.currentFrame >= startFrameLeft)
        {
            sprite.currentFrame = sprite.startingFrame;
        }
    }
    else
    {
        sprite.startingFrame = startFrameLeft;
        if (sprite.currentFrame < sprite.startingFrame || sprite.currentFrame >= startFrameLeft + FRAMES_PER_ANIMATION)
        {
            sprite.currentFrame = sprite.startingFrame;
        }
    }
    sprite.playAnimation(delta, FRAMES_PER_ANIMATION, true);
}


void ChargeEnemy::takeHit(int damage)
{
    sprite.startingFrame = HURT_FRAME;
    sprite.currentFrame = HURT_FRAME;
    GameObject::takeHit(damage);
    charging = false;
    if(damage > 0)
    {
        knockedBack = true;
    }
}

ChargeEnemy::~ChargeEnemy()
{
    delete physics;
}

bool ChargeEnemy::checkPlayer()
{
    bool found = false;
    lineOfSight();
    glm::vec2 playerCenter = player.getCenter();

    if (playerCenter.x >= sight.x &&
            playerCenter.x <= sight.x + sight.z &&
            playerCenter.y >= sight.y &&
            playerCenter.y <= sight.y + sight.w)
    {
        found = true;
        Mix_PlayChannel(-1, noticePlayerSound, 0);

    }
    return found;
}

void ChargeEnemy::lineOfSight()
{
    glm::vec2 center = sprite.getCenter();
    glm::vec2 position = sprite.getPosition();

    int range = 8;

    int width = TileManager::TILE_WIDTH;

    for (int i = 1; i <= range; i++)
    {
        glm::vec2 point = glm::vec2(center.x + float(sprite.facing  * width * i), center.y);
        if (!TileManager::tileManager.tileIsPassable(point.x, point.y))
        {
            range = i;
            break;
        }
    }

    float size = range * width;

    sight.y = position.y;
    sight.w = sprite.getSize().y;
    sight.z = size;

    if (sprite.facing  > 0)
    {
        sight.x = center.x;
    }
    else
    {
        sight.x = center.x - size;

    }
}

//charge behaviour input
void ChargeEnemy::input()
{
    if (physics->state == Physics::ON_GROUND)
    {
        //check if player is in range
        if (!charging)
        {
            if (checkPlayer())
            {
                charging = true;
                physics->moveSpeed.y = -3; //do a little hop
                physics->moveSpeed.x = 0;
            }
            else
            {
                //walk behaviour input
                physics->currentSpeed = walkSpeed;
                physics->moveSpeed.x = sprite.facing;
            }
        }
        else
        {
            physics->currentSpeed = chargeSpeed;
            physics->moveSpeed.x = sprite.facing;
        }
    }
}

void ChargeEnemy::checkTurn()
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
            if (!charging || (charging && !checkPlayer()))
            {
                turnAround();
            }
        }
    }
}

void ChargeEnemy::turnAround()
{
    physics->moveSpeed.x *= -1;
    sprite.facing *= -1;

    physics->currentSpeed = walkSpeed;

    charging = false;

    if(armor)
    {
        if(sprite.facing < 0)
        {
            colliders[0].dimensions.x = 0;
        }
        else
        {
            colliders[0].dimensions.x = 2;
        }
    }
}
