#pragma once
#include "Pickup.h"
class PickupHealth :
	public Pickup
{
public:
	PickupHealth(int value, glm::vec2 position, int type, Mix_Chunk* soundEffect);

	int health;

	void apply(Player &player);

	~PickupHealth();
};

