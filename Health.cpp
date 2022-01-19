#include "Health.h"
Health::Health()
{
}

Health::Health(int maxHealth)
{
    currentHealth = maxHealth;
}

bool Health::takeDamage(int damage)
{
    hitTimer = 0.0f;
    damaged = true;
    hit = true;
    currentHealth -= damage;
    if(currentHealth <= 0)
    {
        currentHealth = 0;
        dying = true;
    }
    return dying;
}

void Health::updateDamaged(float delta)
{
    if (damaged)
    {
        hitTimer += delta;

        if (hit && hitTimer >= hitTime)
        {
            hit = false;
        }
        if (hitTimer > damageTime)
        {
            hitTimer = 0.0f;
            damaged = false;
        }
    }
}

void Health::updateDeath(float delta)
{
    deathTimer += delta;
    if(deathTimer >= deathTime)
    {
        dead = true;
    }
}

bool Health::alive()
{
    return !dead && !dying;
}


Health::~Health()
{
}
