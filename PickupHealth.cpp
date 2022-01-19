#include "PickupHealth.h"

PickupHealth::PickupHealth(int value, glm::vec2 position, int type, Mix_Chunk* soundEffect) : health(value), Pickup(position, type, soundEffect)
{
}

void PickupHealth::apply(Player & player)
{
    Pickup::apply(player);
	player.addHealth(health);
	sprite.active = false;
}

PickupHealth::~PickupHealth()
{
}
