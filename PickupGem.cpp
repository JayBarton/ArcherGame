#include "PickupGem.h"


PickupGem::PickupGem(glm::vec2 position, int type, Mix_Chunk* soundEffect) : Pickup(position, type, soundEffect)
{
    sprite.setHitBox(glm::vec4(6, 0, 20, 32));
}

void PickupGem::apply(Player& player)
{
    Pickup::apply(player);
    sprite.active = false;
    player.collectedGems ++;
}


PickupGem::~PickupGem()
{
    //dtor
}
