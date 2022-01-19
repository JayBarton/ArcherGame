#pragma once
#include "Pickup.h"
class PickupLives : public Pickup
{
public:
	PickupLives(glm::vec2 position, int type, Mix_Chunk* soundEffect);

	int lives = 1; //only can get one extra life from a pickup

	void apply(Player &player);

	~PickupLives();
};

