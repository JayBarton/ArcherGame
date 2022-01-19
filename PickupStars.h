#pragma once
#include "Pickup.h"
class PickupStars :
	public Pickup
{
public:
	PickupStars(int value, glm::vec2 position, int type, Mix_Chunk* soundEffect);

	int stars;

	void apply(Player& player);

	~PickupStars();
};

