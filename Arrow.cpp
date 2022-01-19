#include "Arrow.h"
#include "TileManager.h"
#include "SATCollision.h"

Arrow::Arrow()
{
    sprite.setSize(glm::vec2(32, 32)); //not going to be this large
    sprite.setHitBox(glm::vec4(3, 0, 26, 8));
    sprite.active = false;
}

void Arrow::setSounds(const std::vector<Mix_Chunk*>& sounds)
{
    soundEffects = sounds;
}


void Arrow::update(float delta)
{
    if (!stuck)
    {
        glm::vec2 direction = glm::normalize(moveSpeed);

        sprite.rotation = atan2(direction.y, direction.x);

        for(int i = 0; i < 4; i ++)
        {
            sprite.Move(moveSpeed / 4.0f);
            checkTiles();
            if(stuck)
            {
                break;
            }
        }
        if(moveSpeed.x != 0)
        {
            moveSpeed.y += fallAcceleration;
        }
    }
    else
    {
        stuckTimer += delta;
        if (stuckTimer >= stuckTime)
        {
            sprite.active = false;
            sprite.setParent(nullptr);
        }

        if(sprite.parent && sprite.parent->active == false)
        {
            sprite.active = false;
            sprite.setParent(nullptr);
        }
    }
}

void Arrow::fire(glm::vec2 start, int level, glm::vec2 velocity, float fallSpeed)
{
    glm::vec2 s = start - sprite.getHitBoxDimensions() * 0.5f ;
    sprite.SetPosition(s);
    moveSpeed = velocity;
    fallAcceleration = fallSpeed;
    sprite.active = true;
    stuck = false;
    stuckTimer = 0.0f;
    this->level = level;
    previouslyHit.clear();
}

void Arrow::checkObjects(std::vector<GameObject*>& enemies, std::vector<GameObject*>& movables)
{
    if (!stuck)
    {
        bool hit = false;

        glm::vec2 direction = glm::normalize(moveSpeed);

        glm::vec4 hb = sprite.getHitBox();
        glm::vec2 p = sprite.getHitBoxCenter() + direction * 32.0f;
        //Using this to check a bit in front of the arrow
        //prevents collisions whereby an object moves into the arrow from the side
        phb = glm::vec4(p.x - 4, p.y - 4, 8, 8);

        int i = 0;
        while(!hit && i < enemies.size())
        {
            GameObject *enemy = enemies[i];
            //Ask for a second opinion on this, I think it's confusing, but it works how I'd want otherwise...
            i++;

            if(!checkIfHit(enemy))
            {
                hit = checkEnemy(enemy, hb, phb);
            }
        }
        i = 0;
        while(!hit && i < movables.size())
        {
            GameObject *movable = movables[i];
            i++;
            hit = checkMovable(movable, hb, phb);
        }
    }
    else
    {

    }
}

bool Arrow::checkEnemy(GameObject* enemy, glm::vec4 hb, glm::vec4 phb)
{
    glm::vec2 enemyPositon = enemy->sprite.getPosition();

    glm::vec2 greatestPush(0);
    int hitCollider = -1;
    int deepestSweep = 4;

    for (int c = 0; c < enemy->colliders.size(); c++)
    {
        Collider collider = enemy->colliders[c];
        if (collider.type != Collider::WALKABLE && collider.type != Collider::DAMAGE)
        {
            glm::vec2 push = checkObject(enemy, collider, enemyPositon, hb, phb);

            sweep(deepestSweep, push, hb, phb, enemy, collider, enemyPositon, greatestPush, hitCollider, c);
        }
    }

    if(hitCollider >= 0)
    {
        //Since fully charged arrows predict their collision, want to adjust the position of the arrow when
        //they hit armor, otherwise the arrow sprite will not be touching the sprite it collided with
       // if(level == 2 && deepestSweep > 0 && enemy->colliders[hitCollider].type == Collider::ARMOR)
       if(deepestSweep > 0)
        {
            glm::vec2 adjustment = (moveSpeed / 4.0f) * float(deepestSweep);
            sprite.Move(adjustment);
        }
        hitEnemy(enemy, hitCollider);
    }
    return(stuck == true || sprite.active == false);
}

//Used to prevent the arrow from flying through an enemy when moving very fast(at it's max charge level)
//Checks in segments ahead of the arrow and attempts to predict the collision
void Arrow::sweep(int& deepestSweep, glm::vec2& push, glm::vec4& hb, glm::vec4& phb,
                   GameObject* enemy, Collider& collider, glm::vec2& enemyPositon,
                    glm::vec2& greatestPush, int& hitCollider, int c)
{
    int currentSweep = deepestSweep;
    //If push is 0 we have not hit anything
    if (glm::length(push) == 0)
    {
        glm::vec4 hb2 = hb;
        glm::vec4 phb2 = phb;
        glm::vec2 fractionSpeed = moveSpeed / 4.0f;
        //Check 4 segments in front of where the arrow is, using it's movespeed to calculate
        for (currentSweep = 1; currentSweep <= 4; currentSweep++)
        {
            hb2.x += fractionSpeed.x;
            hb2.y += fractionSpeed.y;
            phb2.x += fractionSpeed.x;
            phb2.y += fractionSpeed.y;
            push = checkObject(enemy, collider, enemyPositon, hb2, phb2);
            //We hit an enemy
            if (glm::length(push) != 0)
            {
                break;
            }
        }
    }
    else
    {
        //first attempt at collision was successful
        currentSweep = 0;
    }
    //Want to take the closest collision, so if the current sweep is less than the deepest one, set to that
    if (currentSweep < deepestSweep)
    {
        deepestSweep = currentSweep;
        greatestPush = push;
        hitCollider = c;
    }
    //Similar to how we find the greatest push for slower arrows, but want to make sure the push is
    //not on a deeper sweep
    if (glm::length(push) > glm::length(greatestPush))
    {
        if (currentSweep == deepestSweep)
        {
            greatestPush = push;
            hitCollider = c;
        }
    }
}

void Arrow::hitEnemy(GameObject* enemy, int hitCollider)
{
    Collider collider = enemy->colliders[hitCollider];
    glm::vec2 direction = glm::normalize(moveSpeed);

    if (collider.type == Collider::NORMAL)
    {
        //just for testing, won't be doing this check
        //might actually be doing this check
        if (enemy->physics)
        {
            enemy->physics->push(glm::vec2(direction.x, -4.0f), 4);
        }
        enemy->takeHit(level + 1);
        Mix_PlayChannel(-1, soundEffects[NORMAL_COLLIDE_SOUND], 0);

        if(!enemy->health.dead)
        {
            if (level < 2)
            {
                //Not ideal, but works for now
                if(enemy->colliders[hitCollider].type == Collider::DAMAGE)
                {
                    sprite.active = false;
                }
                else
                {
                    stuck = true;
                    sprite.setParent(&enemy->sprite);
                }
            }
            else
            {
                //Only need to be checking previously hit when arrow is max level
                previouslyHit.push_back(enemy);
            }
        }
        else
        {
            if (level < 2)
            {
                sprite.active = false;
            }
        }
    }
    else if (collider.type == Collider::ARMOR)
    {
        Mix_PlayChannel(-1, soundEffects[ARMOR_COLLIDE_SOUND], 0);
        stuck = true;
        sprite.setParent(&enemy->sprite);
        if (enemy->physics)
        {
            enemy->physics->push(glm::vec2(direction.x, 0.0f), 4);
            enemy->takeHit(0);
        }
    }
    /* else if(collider.type == Collider::BLOCK)
     {
         //No enemies ever have a block collider
         stuck = true;
         sprite.setParent(&enemy->sprite);
     }*/
}


bool Arrow::lineIntersection(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4, glm::vec2& intersection)
{
    float uA = ((p4.x-p3.x)*(p1.y-p3.y) - (p4.y-p3.y)*(p1.x-p3.x)) / ((p4.y-p3.y)*(p2.x-p1.x) - (p4.x-p3.x)*(p2.y-p1.y));
    float uB = ((p2.x-p1.x)*(p1.y-p3.y) - (p2.y-p1.y)*(p1.x-p3.x)) / ((p4.y-p3.y)*(p2.x-p1.x) - (p4.x-p3.x)*(p2.y-p1.y));

    if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
    {
        intersection = glm::vec2(p1.x + (uA * (p2.x - p1.x)), p1.y + (uA * (p2.y-p1.y)));
        return true;

    }
    return false;
}



bool Arrow::checkMovable(GameObject* movable, glm::vec4 hb, glm::vec4 phb)
{
    glm::vec2 movablePosition = movable->sprite.getPosition();

    //Movables should only have the one collider
    Collider collider = movable->colliders[0];

    if(glm::length(checkObject(movable, collider, movablePosition, hb, phb)) > 0)
    {
        stuck = true;
        sprite.setParent(&movable->sprite);
    }
    return stuck;
}

glm::vec2 Arrow::checkObject(GameObject* object, Collider collider, glm::vec2 objectPosition, glm::vec4 hb, glm::vec4 phb)
{
    glm::vec4 ehb = collider.dimensions + glm::vec4(objectPosition.x, objectPosition.y, 0, 0);

    //should i be passing in the object hitbox? probably
    //Should do this check outside of here. Currently being done per collider, but if it's true for one it will be true for all of them
    if (sprite.collide(phb, object->sprite.getHitBox()))
    {
        SimpleShape arrowBox;
        SimpleShape enemyBox;
        //arrowBox = newSquare(sprite.getHitBoxCenter(), hb.z, hb.w * 2); //doubling the size in the y direction to deal with some off collisions
        glm::vec2 center = glm::vec2(hb.x + hb.z * 0.5f, hb.y + hb.w * 0.5f);
        //arrowBox = newSquare(sprite.getHitBoxCenter(), hb.z, hb.w);
        arrowBox = newSquare(center, hb.z, hb.w);

        arrowBox.rotateShape(-sprite.rotation);

        glm::vec2 objectSize = object->sprite.getSize();
        enemyBox = newSquare(object->sprite.getCenter(), objectSize.x, objectSize.y);
        glm::vec2 push(0);
        //Check collision with the enemy itself, and then with its collider
        if(satCollision(arrowBox, enemyBox, push.x, push.y))
        {
            SimpleShape colliderBox;
            colliderBox = newSquare(glm::vec2(ehb.x + ehb.z * 0.5f, ehb.y + ehb.w * 0.5f), ehb.z, ehb.w);
            if(satCollision(arrowBox, colliderBox, push.x, push.y))
            {
                return push;
            }
        }
    }
    return glm::vec2(0);
}

bool Arrow::checkIfHit(GameObject* enemy)
{
    for(int i = 0; i < previouslyHit.size(); i ++)
    {
        if(previouslyHit[i] == enemy)
        {
            return true;
        }
    }
    return false;
}

Arrow::~Arrow()
{
}

void Arrow::checkTiles()
{
    glm::vec2 hitBoxCenter = sprite.getHitBoxCenter();

    float xPosition = hitBoxCenter.x + sprite.getHitBox().z * 0.5f;
    float halfHeight = COLLIDE_OFFSET * 0.5f;

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

void Arrow::nearPhase(glm::vec2 point)
{
    SimpleShape tile = TileManager::tileManager.getTileBox(point.x, point.y);
    glm::vec4 hb = sprite.getHitBox();
    SimpleShape pBox;
    pBox = newSquare(glm::vec2(hb.x + hb.z * 0.5f, hb.y + hb.w * 0.5f), hb.z, COLLIDE_OFFSET);

    pBox.rotateShape(-sprite.rotation);

    glm::vec2 push(0);
    if( satCollision(pBox, tile, push.x, push.y))
    {
        moveSpeed = glm::vec2(0);
        sprite.Move(push);
        stuck = true;
    }
}
