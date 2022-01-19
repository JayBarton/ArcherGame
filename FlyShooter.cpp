#include "FlyShooter.h"

const float FlyShooter::radius = 192.0f;

FlyShooter::FlyShooter(glm::vec2 p, int facing, std::vector<Projectile>* projectiles,
                       std::vector<glm::vec2>& path, Sprite &player, Mix_Chunk* sound) : player(player)
{
    startPosition = p;
    startFacing = facing;
    spawnArea.p = startPosition;
    sprite.setHitBox(glm::vec4(0, 15, 64, 36));

    sprite.setSize(glm::vec2(64, 64));

    flyer = FlyPathBehaviour(2.0f, path);

    shooter.init(8.0f, 1.5f, projectiles, sound);

    addCollider(glm::vec4(0, 15, 64, 36), Collider::NORMAL);

    init(p, facing);

    sprite.startingFrame = 0;

}

void FlyShooter::update(float delta)
{
    if(!health.hit)
    {
        if (shooter.update(delta))
        {
           // glm::vec2 center = sprite.getCenter() - (glm::vec2(32.0f, 16.0f) * 0.5f);
            //glm::vec2 directionToPlayer = glm::normalize(player.getCenter() - center);
            glm::vec2 directionToPlayer = glm::normalize(player.getCenter() - sprite.getCenter());

            shooter.fire(directionToPlayer, sprite.getCenter(), 0);
        }
        flyer.update(delta, sprite);
        sprite.playAnimation(delta, 4, true);
    }

    health.updateDamaged(delta);

}

void FlyShooter::takeHit(int damage)
{
    GameObject::takeHit(damage);
    shooter.fireTimer = 0.0f;
    sprite.currentFrame = DAMAGE_FRAME;
}

void FlyShooter::reset()
{
    flyer.reset();
    init(startPosition, startFacing);
}

void FlyShooter::init(glm::vec2 p, int facing)
{
    sprite.active = true;
    sprite.SetPosition(p);
    sprite.facing = facing;

    health = Health(3);

    shooter.fireTimer = 0.0f;

}


FlyShooter::~FlyShooter()
{
}
