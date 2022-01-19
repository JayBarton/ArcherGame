#pragma once
#include <map>
#include "Collider.h"
#include "Sprite.h"
#include "Physics.h"
#include "ProjectileShooter.h"
#include "Health.h"

#include <string>

//Currenty, p is equal to the startPosition of everything that uses this
//If this does not change, remove the struct, make startPosition a variable for this class
//remove startPosition from enemy/block classes, and make a public spawnEntered bool in this class
struct SpawnArea
{
    glm::vec2 p;
    bool entered = false;
};

class Collider;
class GameObject
{
public:
    GameObject();

    Sprite sprite; //may replace this with a transform/position type of thing
    Health health;
    Physics *physics = nullptr;

    SpawnArea spawnArea; //Not sure how to handle this, here for now
    glm::vec2 despawnArea = glm::vec2(128, 128);

    std::vector<Collider> colliders;

    int addCollider(glm::vec4, int type);
    virtual void takeHit(int damage)
    {
        health.takeDamage(damage);
    }

    virtual void handleDeath(float deltaTime)
    {
        health.updateDeath(deltaTime);
    }

    void destroy();

    virtual void update(float delta) {}

    virtual void init(glm::vec2 p, int facing) {}

    virtual void reset() {}

    virtual ~GameObject();
};

