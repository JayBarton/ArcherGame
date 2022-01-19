#include "Projectile.h"
#include "TileManager.h"

Projectile::Projectile()
{
    sprite.setSize(glm::vec2(32, 16)); //not going to be this large
    sprite.setHitBox(glm::vec4(0, 0, 32, 16));
    sprite.active = false;
}

void Projectile::fire(glm::vec2 start, glm::vec2 velocity, int type)
{
    sprite.SetPosition(start - (sprite.getSize() * 0.5f));
    moveSpeed = velocity;
    direction = glm::normalize(velocity);
    sprite.rotation = atan2(direction.y, direction.x);
    sprite.active = true;
    moving = true;
    sprite.currentFrame = type;
}

void Projectile::update(float delta)
{
    //Want to wait a frame before setting to inactive.
    //Makes the draw look better.
    if(moving)
    {
        for(int i = 0; i < 4; i ++)
        {
            sprite.Move(moveSpeed/4.0f);

            checkTiles();
            //find a better way!
            if(!moving)
            {
                break;
            }
        }
    }
    else
    {
        sprite.active = false;
    }
}

void Projectile::checkTiles()
{
    glm::vec2 hitBoxCenter = sprite.getHitBoxCenter();

    float xPosition = hitBoxCenter.x + sprite.getHitBox().z * 0.5f;
    float halfHeight = 16 * 0.5f;

    glm::vec2 point1 = glm::vec2(xPosition, hitBoxCenter.y - halfHeight);
    glm::vec2 point2 = glm::vec2(xPosition, hitBoxCenter.y + halfHeight);

    point1 = sprite.rotateVector(point1, hitBoxCenter, sprite.rotation);
    point2 = sprite.rotateVector(point2, hitBoxCenter, sprite.rotation);

    if ((moveSpeed.y > 0 &&  !TileManager::tileManager.airPassable(point1.x, point1.y))  ||
            !TileManager::tileManager.tileIsPassable(point1.x, point1.y))
    {
        nearPhase(point1);
    }
    else if ((moveSpeed.y > 0 &&  !TileManager::tileManager.airPassable(point2.x, point2.y))  ||
             !TileManager::tileManager.tileIsPassable(point2.x, point2.y))
    {
        nearPhase(point2);
    }
}

void Projectile::nearPhase(glm::vec2 point)
{
    SimpleShape tile = TileManager::tileManager.getTileBox(point.x, point.y);
    glm::vec4 hb = sprite.getHitBox();
    SimpleShape pBox;
    pBox = newSquare(glm::vec2(hb.x + hb.z * 0.5f, hb.y + hb.w * 0.5f), hb.z, 16);

    pBox.rotateShape(-sprite.rotation);

    glm::vec2 push(0);
    if(satCollision(pBox, tile, push.x, push.y))
    {
        moveSpeed = glm::vec2(0);
        //sprite.Move(push);
        moving = false;
    }
}

void Projectile::checkMovables(std::vector<GameObject*>& movables)
{
    glm::vec2 direction = glm::normalize(moveSpeed);

    glm::vec4 hb = sprite.getHitBox();
    glm::vec2 p = sprite.getHitBoxCenter() + direction * 32.0f;
    glm::vec4 phb = glm::vec4(p.x - 4, p.y - 4, 8, 8);
    int i = 0;
    while(moving && i < movables.size())
    {
        GameObject *movable = movables[i];
        i++;
        checkMovable(movable, hb, phb);
    }
}

void Projectile::checkMovable(GameObject* movable, glm::vec4 hb, glm::vec4 phb)
{
    glm::vec2 movablePosition = movable->sprite.getPosition();

    //Movables should only have the one collider
    Collider collider = movable->colliders[0];

    if(glm::length(checkObject(movable, collider, movablePosition, hb, phb)) > 0)
    {
        moving = false;
    }
}

glm::vec2 Projectile::checkObject(GameObject* object, Collider collider, glm::vec2 objectPosition, glm::vec4 hb, glm::vec4 phb)
{
    glm::vec4 ehb = collider.dimensions + glm::vec4(objectPosition.x, objectPosition.y, 0, 0);
    if (sprite.collide(phb, ehb))
    {
        SimpleShape arrowBox;
        SimpleShape enemyBox;
        arrowBox = newSquare(sprite.getHitBoxCenter(), hb.z, hb.w);
        arrowBox.rotateShape(-sprite.rotation);

        glm::vec2 objectSize = object->sprite.getSize();
        enemyBox = newSquare(object->sprite.getCenter(), objectSize.x, objectSize.y);
        glm::vec2 push(0);
        //Check collision with the enemy itself, and then with its collider
        if(satCollision(arrowBox, enemyBox, push.x, push.y))
        {
            SimpleShape colliderBox;
            colliderBox = newSquare(glm::vec2(ehb.x + ehb.z *0.5f, ehb.y + ehb.w * 0.5f), ehb.z, ehb.w);
            if(satCollision(arrowBox, colliderBox, push.x, push.y))
            {
                return push;
            }
        }
    }
    return glm::vec2(0);
}


Projectile::~Projectile()
{
}
