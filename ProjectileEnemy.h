#pragma once
#include "Sprite.h"
#include "Projectile.h"
#include "ProjectileShooter.h"
#include "GameObject.h"

class Camera;
class ProjectileEnemy : public GameObject
{
public:
	ProjectileEnemy(glm::vec2 p, int facing, bool armor, std::vector<Projectile> *projectiles, Camera &camera, Sprite &player, Mix_Chunk* sound);

	ProjectileShooter shooter;
	Sprite &player;
	Camera &camera;

	float xPosition; //Never going to move, so just save the x position on creation

	bool playerInRange = false;

	void update(float delta);

	void takeHit(int damage);

	void reset();

	~ProjectileEnemy();
};

