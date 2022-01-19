#include "ProjectileShooter.h"

void ProjectileShooter::init(float speed, float fireTime, std::vector<Projectile>* projectiles, Mix_Chunk* sound)
{
	this->speed = speed;
	this->fireTime = fireTime;
	this->projectiles = projectiles;
	fireSound = sound;
}


bool ProjectileShooter::update(float delta)
{
	bool firing = false;
	fireTimer += delta;
	if (fireTimer >= fireTime)
	{
		//fire
		fireTimer = 0.0f;
		firing = true;
	}
	return firing;
}

void ProjectileShooter::fire(const glm::vec2 & direction, const glm::vec2 &firePosition, int type)
{
	for (int i = 0; i < projectiles->size(); i++)
	{
		if (!(*projectiles)[i].sprite.active)
		{
			(*projectiles)[i].fire(firePosition, direction * speed, type);
			Mix_PlayChannel(6 + type, fireSound, 0);
			break;
		}
	}
}

ProjectileShooter::~ProjectileShooter()
{
}
