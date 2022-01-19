#include "ProjectileEnemy.h"
#include "Camera.h"

ProjectileEnemy::ProjectileEnemy(glm::vec2 p, int facing, bool armor, std::vector<Projectile>* projectiles, Camera &camera, Sprite & player, Mix_Chunk* sound) : player(player), camera(camera)
{
    sprite.active = true;
    sprite.SetPosition(p);

    spawnArea.p = p;
    xPosition = p.x;

    sprite.setHitBox(glm::vec4(0, 0, 32, 64));

    sprite.setSize(glm::vec2(32, 64));
    sprite.facing = facing;

    shooter.init(8.0f, 1.5f, projectiles, sound);

    //TODO Get these numbers correct, I want to be able to hit them while standing but not while ducking.
    if(armor)
    {
        addCollider(glm::vec4(0, 52, 32, 12), 1);
    }
    addCollider(glm::vec4(0, 0, 32, 64), 0);

    health = Health(3);
}

void ProjectileEnemy::update(float delta)
{
    if(camera.onScreen(sprite.getCenter(), sprite.getSize()))
    {
        if(!playerInRange)
        {
            playerInRange = true;
            shooter.fireTimer = 0.0f;
        }
    }
    else
    {
        playerInRange = false;
    }
    if(!health.hit)
    {
        if(playerInRange)
        {
            if (shooter.update(delta))
            {
                glm::vec2 fireDirection = glm::vec2(sprite.facing, 0.0f);
                glm::vec2 firePosition = sprite.getCenter();
                shooter.fire(fireDirection, firePosition, 1);
            }
        }
    }
    if(!health.damaged)
    {
        float playerX = player.getPosition().x;

        if (playerX < xPosition)
        {
            sprite.facing = -1;
            sprite.currentFrame = 0;
        }
        else
        {
            sprite.facing = 1;
            sprite.currentFrame = 1;
        }
    }
    else
    {
        //Play damaged animation
        sprite.currentFrame = 2;
    }
    health.updateDamaged(delta);
}

void ProjectileEnemy::takeHit(int damage)
{
    GameObject::takeHit(damage);
    shooter.fireTimer = 0.0f;
    //Begin the hurt animation here
    sprite.currentFrame = 2;
}

void ProjectileEnemy::reset()
{
    sprite.active = true;
    health = Health(3);
    shooter.fireTimer = 0.0f;
}


ProjectileEnemy::~ProjectileEnemy()
{
}
