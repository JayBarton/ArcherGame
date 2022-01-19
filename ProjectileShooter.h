#pragma once
#include <glm.hpp>
#include "Projectile.h"
#include "Sprite.h"
#include "SDL_mixer.h"
class Projectile;
class ProjectileShooter
{
public:
	ProjectileShooter()
	{
	}

	void init(float speed, float fireTime, std::vector<Projectile> *projectiles, Mix_Chunk* sound);

	std::vector<Projectile> *projectiles = nullptr;

	//the speed the projectiles fire at
	float speed;

	//How frequently projectiles are fired
	float fireTimer = 0.0f;
	float fireTime;

	Mix_Chunk* fireSound;

	bool update(float delta);
	void fire(const glm::vec2& direction, const glm::vec2 &firePosition, int type);

	~ProjectileShooter();
};

