#pragma once
#include "Sprite.h"
#include "Projectile.h"
#include "ProjectileShooter.h"
#include "GameObject.h"
#include "FlyPathBehaviour.h"

class FlyShooter : public GameObject
{
public:
	FlyShooter(glm::vec2 p, int facing, std::vector<Projectile> *projectiles,
		std::vector<glm::vec2>& path, Sprite &player, Mix_Chunk* sound);

	static const float radius; //how close the player must be for the enemy to fire

	const static int DAMAGE_FRAME = 4;

	ProjectileShooter shooter;
	FlyPathBehaviour flyer;

	Sprite &player;

	glm::vec2 startPosition;

	int startFacing;

	float speed = 2.0f;

	void update(float delta);

	void takeHit(int damage);

	void reset();
	void init(glm::vec2 p, int facing);


	~FlyShooter();
};

