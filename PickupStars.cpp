#include "PickupStars.h"

PickupStars::PickupStars(int value, glm::vec2 position, int type, Mix_Chunk* soundEffect) : stars(value), Pickup(position, type, soundEffect)
{
    if(value == 1)
    {
        sprite.setHitBox(glm::vec4(7, 5, 18, 17));
    }
    else if(value == 5)
    {
        sprite.setHitBox(glm::vec4(4, 3, 24, 25));
    }
}

void PickupStars::apply(Player & player)
{
    Pickup::apply(player);
	player.addStars(stars);
	sprite.active = false;
}

PickupStars::~PickupStars()
{
}
