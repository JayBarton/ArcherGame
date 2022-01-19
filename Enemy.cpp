#include "Enemy.h"
#include "TileManager.h"
#include "SATCollision.h"

int Enemy::addCollider(glm::vec4 dims, int type)
{
	Collider c(dims, type);
	colliders.push_back(c);
	return colliders.size() - 1;
}
