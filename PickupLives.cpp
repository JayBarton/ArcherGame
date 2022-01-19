#include "PickupLives.h"



PickupLives::PickupLives(glm::vec2 position, int type, Mix_Chunk* soundEffect) : Pickup(position, type, soundEffect)
{
}


void PickupLives::apply(Player & player)
{
    Pickup::apply(player);
	player.addLives(lives);
	sprite.active = false;
}

PickupLives::~PickupLives()
{
}
